import { AppRandom } from '../src/math';
import { TEST_PREAMBLE } from './preamble';

test('Seeded Random', () => {
    TEST_PREAMBLE();

    AppRandom.Get.init(0);
    expect(AppRandom.Get.random()).toEqual(0.7803563384230067);
    expect(AppRandom.Get.random()).toEqual(0.05144520047760746);
    expect(AppRandom.Get.random()).toEqual(0.5317574394273067);

    AppRandom.Get.init(12345);
    expect(AppRandom.Get.random()).toEqual(0.20703519639616447);
    expect(AppRandom.Get.random()).toEqual(0.6602710883040208);
    expect(AppRandom.Get.random()).toEqual(0.500949276170095);

    AppRandom.Get.init(0);
    expect(AppRandom.Get.random()).toEqual(0.7803563384230067);
    expect(AppRandom.Get.random()).toEqual(0.05144520047760746);
    expect(AppRandom.Get.random()).toEqual(0.5317574394273067);
});
