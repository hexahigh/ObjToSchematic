import { Mesh } from './mesh';

export abstract class IImporter {
    public abstract parseFile(filePath: string): void;
    public abstract toMesh(): Mesh;
    public supports(fileExtension: string) {
        return this.getSupportedFileExtensions().includes(fileExtension);
    }

    protected abstract getSupportedFileExtensions(): string[];
}
