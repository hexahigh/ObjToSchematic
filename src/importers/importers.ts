import { Mesh } from '../mesh';
import { VoxelMesh } from '../voxel_mesh';
import { IFileImporter } from './base_importer';
import { ObjImporter } from './obj_importer';
import { VoxImporter } from './vox_importer';

export type TImporters = 'obj' | 'vox';

export class ImporterFactor {
    public static GetImporter(importer: TImporters, filename: string): { type: 'Mesh', importer: IFileImporter<Mesh> } | { type: 'VoxelMesh', importer: IFileImporter<VoxelMesh> } {
        switch (importer) {
            case 'obj':
                return {
                    type: 'Mesh',
                    importer: new ObjImporter(filename),
                };
            case 'vox':
                return {
                    type: 'VoxelMesh',
                    importer: new VoxImporter(filename),
                };
        }
    }
}
