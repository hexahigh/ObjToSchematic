import { ObjImporter } from './importers/obj_importer';
import { FbxImporter } from './importers/fbx_importer';
import { AppError, ASSERT } from './util';

import path from 'path';

export class MeshImporter {
    public static import(filepath: string) {
        ASSERT(path.isAbsolute(filepath));
        const parsedPath = path.parse(filepath);

        const importers = [new ObjImporter(), new FbxImporter()];
        for (const importer of importers) {
            if (importer.supports(parsedPath.ext.toLowerCase())) {
                importer.parseFile(filepath);
                return importer.toMesh();
            }
        }

        throw new AppError(`Could not load file-type '${parsedPath.ext.toLowerCase()}' when loading ${parsedPath.base}`);
    }
}
