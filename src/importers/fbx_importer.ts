import { IImporter } from './base_importer';
import { MaterialType, Mesh, SolidMaterial, TexturedMaterial, Tri } from '../mesh';
import { Vector3 } from '../vector';
import { UV, RGB, LOG, ASSERT } from '../util';

import fs from 'fs';

import * as FBXParser from 'fbx-parser';

export class FbxImporter extends IImporter {
    private _vertices: Vector3[] = [];
    private _normals: Vector3[] = [];
    private _uvs: UV[] = [];
    private _tris: Tri[] = [];
    private _materials: {[key: string]: (SolidMaterial | TexturedMaterial)} = {
        'DEFAULT_UNASSIGNED': { type: MaterialType.solid, colour: RGB.white },
    };

    public override parseFile(filePath: string) {
        let fbx: FBXParser.FBXData;
        try {
            fbx = FBXParser.parseBinary(fs.readFileSync(filePath));
        } catch (e) {
            fbx = FBXParser.parseText(fs.readFileSync(filePath, 'utf-8'));
        }
        const root = new FBXParser.FBXReader(fbx);

        // Parse vertex data
        const _vertices = root.node('Objects')?.node('Geometry')?.node('Vertices')?.prop(0, 'number[]');
        ASSERT(_vertices, 'Could not find vertex data node');
        ASSERT(_vertices.length % 3 === 0, 'Expected vertices array to be a multiple of 3');
        const numVertices = _vertices.length / 3;
        this._vertices = Array<Vector3>(numVertices);
        for (let i = 0; i < numVertices; ++i) {
            this._vertices[i] = new Vector3(
                _vertices[i * 3 + 0],
                _vertices[i * 3 + 1],
                _vertices[i * 3 + 2],
            );
        }
        // LOG('vertices', this._vertices);

        // Parse face data
        const _faces = root.node('Objects')?.node('Geometry')?.node('PolygonVertexIndex')?.prop(0, 'number[]');
        ASSERT(_faces, 'Could not find face data node');
        
        let base = [];
        for (let i = 0; i < _faces.length; ++i) {
            base.push(_faces[i]);
            if (_faces[i] < 0) {
                this._addFaceData(base);
                base = [];
            }
        }
        // LOG('tris', this._tris);

        LOG(root);
    }

    private _addFaceData(points: number[]) {
        // Make all indices positive
        for (let i = 0; i < points.length; ++i) {
            if (points[i] < 0) {
                points[i] = (-points[i]) - 1;
            }
        }

        const pointBase = points[0];
        for (let i = 1; i < points.length - 1; ++i) {
            const pointA = points[i];
            const pointB = points[i+1];
            const tri: Tri = {
                positionIndices: {
                    x: pointBase,
                    y: pointA,
                    z: pointB,
                },
                material: 'None',
            };
            this._tris.push(tri);
        }
    }

    public override toMesh(): Mesh {
        return new Mesh(this._vertices, this._normals, this._uvs, this._tris, this._materials);
    }

    public override getSupportedFileExtensions(): string[] {
        return ['.fbx'];
    }
}
