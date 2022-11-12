import { EFaceVisibility } from './block_assigner';
import { Bounds } from './bounds';
import { ChunkedBufferGenerator, TVoxelMeshBufferDescription } from './buffer';
import { RGBA, RGBAUtil } from './colour';
import { OcclusionManager } from './occlusion';
import { TOptional } from './util';
import { ASSERT } from './util/error_util';
import { LOGF } from './util/log_util';
import { Vector3 } from './vector';
import { RenderNextVoxelMeshChunkParams, VoxeliseParams } from './worker_types';

const addon = require('bindings')('addon');

export interface Voxel {
    position: Vector3;
    colour: RGBA;
}

export type TVoxelOverlapRule = 'first' | 'average';

export type TVoxelMeshParams = Pick<VoxeliseParams.Input, 'voxelOverlapRule' | 'enableAmbientOcclusion'>;

export class VoxelMesh {
    private _internal: any;
    private _voxelMeshParams: TVoxelMeshParams;

    public constructor(voxelMeshParams: TVoxelMeshParams) {
        this._voxelMeshParams = voxelMeshParams;

        this._internal = new addon.VoxelMesh();
    }

    public getInternal() {
        return this._internal;
    }

    public isVoxelAt(pos: Vector3) {
        return this._internal.isVoxelAt(pos.x, pos.y, pos.z);
    }

    public isOpaqueVoxelAt(pos: Vector3) {
        const voxel = this.getVoxelAt(pos);
        if (voxel) {
            return voxel.colour.a == 1.0;
        }
        return false;
    }

    public addVoxel(pos: Vector3, colour: RGBA) {

    }

    public getVoxelAt(pos: Vector3): TOptional<Voxel> {
        return undefined;
        /*
        ASSERT(this._hasReadRawData);
        const voxelIndex = this._internal.GetVoxelIndex(pos.x, pos.y, pos.z);
        if (voxelIndex !== -1) {
            const voxelPosition = Vector3.fromArray(this._rawPositions.subarray(voxelIndex * 3, voxelIndex * 3 + 3));
            const voxelColour = RGBAUtil.fromArray(this._rawColours.subarray(voxelIndex * 4, voxelIndex * 4 + 4));

            return {
                colour: voxelColour,
                position: voxelPosition,
            };
        }
        */
    }

    public static getFullFaceVisibility(): EFaceVisibility {
        return EFaceVisibility.Up | EFaceVisibility.Down | EFaceVisibility.North | EFaceVisibility.West | EFaceVisibility.East | EFaceVisibility.South;
    }

    public getFaceVisibility(pos: Vector3) {
        let visibility: EFaceVisibility = 0;
        if (!this.isOpaqueVoxelAt(Vector3.add(pos, new Vector3(0, 1, 0)))) {
            visibility += EFaceVisibility.Up;
        }
        if (!this.isOpaqueVoxelAt(Vector3.add(pos, new Vector3(0, -1, 0)))) {
            visibility += EFaceVisibility.Down;
        }
        if (!this.isOpaqueVoxelAt(Vector3.add(pos, new Vector3(1, 0, 0)))) {
            visibility += EFaceVisibility.North;
        }
        if (!this.isOpaqueVoxelAt(Vector3.add(pos, new Vector3(-1, 0, 0)))) {
            visibility += EFaceVisibility.South;
        }
        if (!this.isOpaqueVoxelAt(Vector3.add(pos, new Vector3(0, 0, 1)))) {
            visibility += EFaceVisibility.East;
        }
        if (!this.isOpaqueVoxelAt(Vector3.add(pos, new Vector3(0, 0, -1)))) {
            visibility += EFaceVisibility.West;
        }
        return visibility;
    }

    public getBounds(): Bounds {
        const boundsEncoded: Int32Array = this._internal.getVoxelBounds();
        return new Bounds(new Vector3(boundsEncoded[0], boundsEncoded[1], boundsEncoded[2]), new Vector3(boundsEncoded[3], boundsEncoded[4], boundsEncoded[5]));
    }

    public getVoxelCount() {
        return this._internal.getVoxelCount();
    }

    public getNeighbours(pos: Vector3) {
        ASSERT(this._voxelMeshParams.enableAmbientOcclusion, 'Ambient occlusion is disabled');

        //const value = this._internal.getNeighbours(pos.x, pos.y, pos.z);

        // TODO: Fix
        return { value: 0 };
    }

    /*
     * Returns true if a voxel at position 'pos' has a neighbour with offset 'offset'
     * Offset must be a vector that exists within this._neighbours defined above
     */
    public hasNeighbour(pos: Vector3, offset: Vector3): boolean {
        return (this.getNeighbours(pos).value & (1 << OcclusionManager.getNeighbourIndex(offset))) > 0;
    }

    private _renderParams?: RenderNextVoxelMeshChunkParams.Input;
    public setRenderParams(params: RenderNextVoxelMeshChunkParams.Input) {
        this._renderParams = params;
        this._bufferChunks = [];
    }

    private _bufferChunks: Array<TVoxelMeshBufferDescription & { moreVoxelsToBuffer: boolean, progress: number }> = [];
    public getChunkedBuffer(chunkIndex: number): TVoxelMeshBufferDescription & { moreVoxelsToBuffer: boolean, progress: number } {
        ASSERT(this._renderParams, 'Called VoxelMesh.getChunkedBuffer() without setting render params');
        if (this._bufferChunks[chunkIndex] === undefined) {
            LOGF(`[VoxelMesh]: getChunkedBuffer: ci: ${chunkIndex} not cached`);
            this._bufferChunks[chunkIndex] = ChunkedBufferGenerator.fromVoxelMesh(this, this._renderParams, chunkIndex);
        } else {
            LOGF(`[VoxelMesh]: getChunkedBuffer: ci: ${chunkIndex} cached`);
        }
        return this._bufferChunks[chunkIndex];
    }
}
