import { StatusHandler } from '../src/status';
import { ASSERT } from '../src/util/error_util';
import { LOG_MAJOR, Logger, TIME_END, TIME_START } from '../src/util/log_util';
import { WorkerClient } from '../src/worker_client';
import { AssignParams, ExportParams, ImportParams, TFromWorkerMessage, VoxeliseParams } from '../src/worker_types';

export type THeadlessConfig = {
    import: ImportParams.Input,
    voxelise: VoxeliseParams.Input,
    assign: AssignParams.Input,
    export: ExportParams.Input,
    debug: {
        showLogs: boolean,
        showWarnings: boolean,
        showTimings: boolean,
    }
}

export function runHeadless(headlessConfig: THeadlessConfig) {
    if (headlessConfig.debug.showLogs) {
        Logger.Get.enableLOGMAJOR();
    }
    if (headlessConfig.debug.showWarnings) {
        Logger.Get.enableLOGWARN();
    }
    if (headlessConfig.debug.showTimings) {
        Logger.Get.enableLOGTIME();
    }

    {
        TIME_START('[TIMER] Importer');
        LOG_MAJOR('\nImporting...');
        WorkerClient.Get.import(headlessConfig.import, () => {
            StatusHandler.Get.dump().clear();
            TIME_END('[TIMER] Importer');
            onImporterFinished(headlessConfig);
        });
    }
}

function onImporterFinished(headlessConfig: THeadlessConfig) {
    TIME_START('[TIMER] Voxeliser');
    LOG_MAJOR('\nVoxelising...');
    WorkerClient.Get.voxelise(headlessConfig.voxelise, () => {
        StatusHandler.Get.dump().clear();
        TIME_END('[TIMER] Voxeliser');
        onVoxeliserFinished(headlessConfig);
    });
}

function onVoxeliserFinished(headlessConfig: THeadlessConfig) {
    TIME_START('[TIMER] Assigner');
    LOG_MAJOR('\nAssigning...');
    WorkerClient.Get.assign(headlessConfig.assign, () => {
        StatusHandler.Get.dump().clear();
        TIME_END('[TIMER] Assigner');
        onAssignerFinished(headlessConfig);
    });
}

function onAssignerFinished(headlessConfig: THeadlessConfig) {
    TIME_START('[TIMER] Exporter');
    LOG_MAJOR('\nExporting...');

    const onReadyToExport = () => {
        WorkerClient.Get.export(headlessConfig.export, () => {
            StatusHandler.Get.dump().clear();
            TIME_END('[TIMER] Exporter');
        });
    };

    /**
     * The OBJExporter is unique in that it uses the actual render buffer used by WebGL
     * to create its data, in headless mode this render buffer is not created so we must
     * generate it manually
     */
    if (headlessConfig.export.exporter === 'obj') {
        const gatherChunks = () => {
            WorkerClient.Get.renderChunkedVoxelMesh({
                enableAmbientOcclusion: headlessConfig.voxelise.enableAmbientOcclusion,
                desiredHeight: headlessConfig.voxelise.size,
            }, (result: TFromWorkerMessage) => {
                ASSERT(result.action === 'RenderNextVoxelMeshChunk', `Expected RenderNextVoxelMeshChunk, Got ${result.action}`);
                if (result.result.moreVoxelsToBuffer) {
                    gatherChunks();
                } else {
                    onReadyToExport();
                }
            });
        };

        gatherChunks();
    } else {
        onReadyToExport();
    }
}
