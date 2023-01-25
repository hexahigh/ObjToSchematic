import { Atlas } from './atlas';
import { BlockMesh } from './block_mesh';
import { BufferGenerator } from './buffer';
import { EAppEvent, EventManager } from './event';
import { IExporter } from './exporters/base_exporter';
import { ExporterFactory } from './exporters/exporters';
import { ObjImporter } from './importers/obj_importer';
import { Mesh } from './mesh';
import { ProgressManager, TTaskHandle } from './progress';
import { StatusHandler } from './status';
import { ASSERT } from './util/error_util';
import { Logger } from './util/log_util';
import { VoxelMesh } from './voxel_mesh';
import { IVoxeliser } from './voxelisers/base-voxeliser';
import { VoxeliserFactory } from './voxelisers/voxelisers';
import { AssignParams, ExportParams, ImportParams, InitParams, RenderMeshParams, RenderNextBlockMeshChunkParams, RenderNextVoxelMeshChunkParams, SetMaterialsParams, TFromWorkerMessage, VoxeliseParams } from './worker_types';

export class WorkerClient {
    private static _instance: WorkerClient;
    public static get Get() {
        return this._instance || (this._instance = new this());
    }

    private constructor() {
        Logger.Get.enableLogToFile();
        Logger.Get.initLogFile('worker');
    }

    private _loadedMesh?: Mesh;
    private _loadedVoxelMesh?: VoxelMesh;
    private _loadedBlockMesh?: BlockMesh;

    /**
     * This function should only be called if the client is using the worker.
     */
    public init(params: InitParams.Input, onFinish: (result: TFromWorkerMessage) => void): void {
        EventManager.Get.add(EAppEvent.onTaskStart, (e: any) => {
            const message: TFromWorkerMessage = {
                action: 'Progress',
                payload: {
                    type: 'Started',
                    taskId: e[0],
                },
            };
            postMessage(message);
        });

        EventManager.Get.add(EAppEvent.onTaskProgress, (e: any) => {
            const message: TFromWorkerMessage = {
                action: 'Progress',
                payload: {
                    type: 'Progress',
                    taskId: e[0],
                    percentage: e[1],
                },
            };
            postMessage(message);
        });

        EventManager.Get.add(EAppEvent.onTaskEnd, (e: any) => {
            const message: TFromWorkerMessage = {
                action: 'Progress',
                payload: {
                    type: 'Finished',
                    taskId: e[0],
                },
            };
            postMessage(message);
        });

        onFinish({
            action: 'Init',
            statusMessages: StatusHandler.Get.getAllStatusMessages(),
            result: {},
        });
    }

    public import(params: ImportParams.Input, onFinish: (result: TFromWorkerMessage) => void): void {
        const importer = new ObjImporter();
        importer.parseFile(params.filepath);
        this._loadedMesh = importer.toMesh();
        this._loadedMesh.processMesh(params.rotation.y, params.rotation.x, params.rotation.z);

        onFinish({
            action: 'Import',
            statusMessages: StatusHandler.Get.getAllStatusMessages(),
            result: {
                triangleCount: this._loadedMesh.getTriangleCount(),
                dimensions: this._loadedMesh.getBounds().getDimensions(),
                materials: this._loadedMesh.getMaterials(),
            },
        });
    }

    public setMaterials(params: SetMaterialsParams.Input, onFinish: (result: TFromWorkerMessage) => void): void {
        ASSERT(this._loadedMesh !== undefined);

        this._loadedMesh.setMaterials(params.materials);

        onFinish({
            action: 'SetMaterials',
            statusMessages: StatusHandler.Get.getAllStatusMessages(),
            result: {
                materials: this._loadedMesh.getMaterials(),
                materialsChanged: Array.from(params.materials.keys()), // TODO: Change to actual materials changed
            },
        });
    }

    public renderMesh(params: RenderMeshParams.Input, onFinish: (result: TFromWorkerMessage) => void): void {
        ASSERT(this._loadedMesh !== undefined);

        onFinish({
            action: 'RenderMesh',
            statusMessages: StatusHandler.Get.getAllStatusMessages(),
            result: {
                buffers: BufferGenerator.fromMesh(this._loadedMesh),
                dimensions: this._loadedMesh.getBounds().getDimensions(),
            },
        });
    }


    public voxelise(params: VoxeliseParams.Input, onFinish: (result: TFromWorkerMessage) => void): void {
        ASSERT(this._loadedMesh !== undefined);

        const voxeliser: IVoxeliser = VoxeliserFactory.GetVoxeliser(params.voxeliser);
        this._loadedVoxelMesh = voxeliser.voxelise(this._loadedMesh, params);

        this._voxelMeshChunkIndex = 0;

        onFinish({
            action: 'Voxelise',
            statusMessages: StatusHandler.Get.getAllStatusMessages(),
            result: {
            },
        });
    }

    private _voxelMeshChunkIndex = 0;
    private _voxelMeshProgressHandle?: TTaskHandle;
    public renderChunkedVoxelMesh(params: RenderNextVoxelMeshChunkParams.Input, onFinish: (result: TFromWorkerMessage) => void): void {
        ASSERT(this._loadedVoxelMesh !== undefined);

        const isFirstChunk = this._voxelMeshChunkIndex === 0;
        if (isFirstChunk) {
            this._voxelMeshProgressHandle = ProgressManager.Get.start('VoxelMeshBuffer');
            this._loadedVoxelMesh.setRenderParams(params);
        }

        const buffer = this._loadedVoxelMesh.getChunkedBuffer(this._voxelMeshChunkIndex);
        ++this._voxelMeshChunkIndex;

        if (this._voxelMeshProgressHandle !== undefined) {
            if (buffer.moreVoxelsToBuffer) {
                ProgressManager.Get.progress(this._voxelMeshProgressHandle, buffer.progress);
            } else {
                ProgressManager.Get.end(this._voxelMeshProgressHandle);
                this._voxelMeshProgressHandle = undefined;
            }
        }

        onFinish({
            action: 'RenderNextVoxelMeshChunk',
            statusMessages: StatusHandler.Get.getAllStatusMessages(),
            result: {
                buffer: buffer,
                dimensions: this._loadedVoxelMesh.getBounds().getDimensions(),
                voxelSize: 8.0 / params.desiredHeight,
                moreVoxelsToBuffer: buffer.moreVoxelsToBuffer,
                isFirstChunk: isFirstChunk,
            },
        });
    }

    public assign(params: AssignParams.Input, onFinish: (result: TFromWorkerMessage) => void): void {
        ASSERT(this._loadedVoxelMesh !== undefined);

        this._loadedBlockMesh = BlockMesh.createFromVoxelMesh(this._loadedVoxelMesh, params);

        this._blockMeshChunkIndex = 0;

        onFinish({
            action: 'Assign',
            statusMessages: StatusHandler.Get.getAllStatusMessages(),
            result: {
            },
        });
    }

    private _blockMeshChunkIndex = 0;
    //private _blockMeshProgressHandle?: TTaskHandle;
    public renderChunkedBlockMesh(params: RenderNextBlockMeshChunkParams.Input, onFinish: (result: TFromWorkerMessage) => void): void {
        ASSERT(this._loadedBlockMesh !== undefined);

        const isFirstChunk = this._blockMeshChunkIndex === 0;
        if (isFirstChunk) {
            //this._blockMeshProgressHandle = ProgressManager.Get.start('BlockMeshBuffer');
        }

        const buffer = this._loadedBlockMesh.getChunkedBuffer(this._blockMeshChunkIndex);
        ++this._blockMeshChunkIndex;

        const atlas = Atlas.load(params.textureAtlas);
        ASSERT(atlas !== undefined);

        onFinish({
            action: 'RenderNextBlockMeshChunk',
            statusMessages: StatusHandler.Get.getAllStatusMessages(),
            result: {
                buffer: buffer,
                dimensions: this._loadedBlockMesh.getVoxelMesh().getBounds().getDimensions(),
                atlasTexturePath: atlas.getAtlasTexturePath(),
                atlasSize: atlas.getAtlasSize(),
                moreBlocksToBuffer: buffer.moreBlocksToBuffer,
                isFirstChunk: isFirstChunk,
            },
        });
    }

    public export(params: ExportParams.Input, onFinish: (result: TFromWorkerMessage) => void): void {
        ASSERT(this._loadedBlockMesh !== undefined);

        const exporter: IExporter = ExporterFactory.GetExporter(params.exporter);
        const fileExtension = '.' + exporter.getFormatFilter().extension;
        if (!params.filepath.endsWith(fileExtension)) {
            params.filepath += fileExtension;
        }
        exporter.export(this._loadedBlockMesh, params.filepath);

        onFinish({
            action: 'Export',
            statusMessages: StatusHandler.Get.getAllStatusMessages(),
            result: {
            },
        });
    }
}
