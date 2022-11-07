const addon = require('bindings')('hello');

export function hello() {
    console.log(addon.hello()); // 'world'
}
