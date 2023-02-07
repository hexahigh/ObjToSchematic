export abstract class IFileImporter<T> {
    protected readonly _filepath: string;

    public constructor(filepath: string) {
        this._filepath = filepath;
    }

    public abstract load(): T;
}
