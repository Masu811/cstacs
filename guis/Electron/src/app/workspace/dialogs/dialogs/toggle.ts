import { Component, computed, input, model, forwardRef, ChangeDetectionStrategy } from '@angular/core';
import { ControlValueAccessor, NG_VALUE_ACCESSOR } from '@angular/forms';

@Component({
  selector: 'toggle',
  templateUrl: 'toggle.html',
  styleUrl: 'toggle.css',
  changeDetection: ChangeDetectionStrategy.OnPush,
  providers: [{
    provide: NG_VALUE_ACCESSOR,
    useExisting: forwardRef(() => Toggle),
    multi: true
  }]
})
export class Toggle implements ControlValueAccessor {

  checked = model<boolean>(false);

  positive = input.required<string>();
  negative = input.required<string>();

  src = computed(() =>
    this.checked() ? this.positive() : this.negative()
  );

  private onChange = (value: boolean) => {};
  private onTouched = () => {};

  writeValue(value: boolean): void {
    this.checked.set(!!value);
  }

  registerOnChange(fn: (value: boolean) => void): void {
    this.onChange = fn;
  }

  registerOnTouched(fn: () => void): void {
    this.onTouched = fn;
  }

  setDisabledState(isDisabled: boolean): void { }

  toggle() {
    const newValue = !this.checked();
    this.checked.set(newValue);
    this.onChange(newValue);
    this.onTouched();
  }
}
