import { ConfigUIElement } from './config_element';

export class NumberUIElement extends ConfigUIElement<number, HTMLInputElement> {
    private _min: number;
    private _max: number;
    private _step: number;
    private _hovering: boolean;

    public constructor() {
        super(Math.floor(Math.random() * Number.MAX_SAFE_INTEGER));
        this._min = 0;
        this._max = 1;
        this._step = 0.1;
        this._hovering = false;
    }

    /**
     * Set the minimum value the input can be set to.
     */
    public setMin(min: number) {
        this._min = min;
        return this;
    }

    /**
     * Set the maximum value the input can be set to.
     */
    public setMax(max: number) {
        this._max = max;
        return this;
    }

    /**
     * Set the number of steps to display the value to.
     */
    public setStep(step: number) {
        this._step = step;
        return this;
    }

    public override registerEvents() {
        this._getElement().addEventListener('change', () => {
            this._setValue(parseInt(this._getElement().value));
        });
    }

    public override _generateInnerHTML() {
        return `
            <input class="number-input" type="number" step="${this._step}" id="${this._getId()}" min="${this._min}" max="${this._max}" value="${this.getValue()}">
        `;
    }

    protected override _onEnabledChanged() {
        super._onEnabledChanged();

        const element = this._getElement();
        element.disabled = !this.getEnabled();
    }

    private _onTypedValue() {
    }

    protected _onValueChanged(): void {    
    }
}
