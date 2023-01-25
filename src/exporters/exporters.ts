import { IExporter } from './base_exporter';
import { Litematic } from './litematic_exporter';
import { NBTExporter } from './nbt_exporter';
import { ObjExporter } from './obj_exporter';
import { SchemExporter } from './schem_exporter';
import { Schematic } from './schematic_exporter';
//import { UncompressedJSONExporter } from './uncompressed_json_exporter';

export type TExporters =
    'schematic' |
    'litematic' |
    'obj' |
    'schem' |
    'nbt';

export class ExporterFactory {
    public static GetExporter(voxeliser: TExporters): IExporter {
        switch (voxeliser) {
            case 'schematic':
                return new Schematic();
            case 'litematic':
                return new Litematic();
            case 'obj':
                return new ObjExporter();
            case 'schem':
                return new SchemExporter();
            case 'nbt':
                return new NBTExporter();
        }
    }
}
