const workerInstance = require('./worker');

addEventListener('message', (e) => {
    const res = workerInstance.doWork(e.data);
    if (res) {
        postMessage(res);
    }
});
