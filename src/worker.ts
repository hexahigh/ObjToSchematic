import { ProgressManager } from './progress';
import { StatusHandler } from './status';
import { AppError } from './util/error_util';
import { WorkerClient } from './worker_client';
import { TFromWorkerMessage, TToWorkerMessage } from './worker_types';

export function doWork(message: TToWorkerMessage, onFinish: (result: TFromWorkerMessage) => void): void {
    StatusHandler.Get.clear();

    if (message.action !== 'RenderNextVoxelMeshChunk' && message.action !== 'RenderNextBlockMeshChunk') {
        ProgressManager.Get.clear();
    }

    try {
        switch (message.action) {
            case 'Init':
                WorkerClient.Get.init(message.params, onFinish);
                break;
            case 'Import':
                WorkerClient.Get.import(message.params, onFinish);
                break;
            case 'SetMaterials':
                WorkerClient.Get.setMaterials(message.params, onFinish);
                break;
            case 'RenderMesh':
                WorkerClient.Get.renderMesh(message.params, onFinish);
                break;
            case 'Voxelise':
                WorkerClient.Get.voxelise(message.params, onFinish);
                break;
            case 'RenderNextVoxelMeshChunk':
                WorkerClient.Get.renderChunkedVoxelMesh(message.params, onFinish);
                break;
            case 'Assign':
                WorkerClient.Get.assign(message.params, onFinish);
                break;
            case 'RenderNextBlockMeshChunk':
                WorkerClient.Get.renderChunkedBlockMesh(message.params, onFinish);
                break;
            case 'Export':
                result: WorkerClient.Get.export(message.params, onFinish);
                break;
        }
    } catch (e: any) {
        onFinish({ action: e instanceof AppError ? 'KnownError' : 'UnknownError', error: e as Error });
    }
}

