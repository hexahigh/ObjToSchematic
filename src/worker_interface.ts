const workerInstance = require('./worker');

addEventListener('message', (e) => {
    workerInstance.doWork(e.data, (result: any) => { // result: TFromWorkerMessage
        postMessage(result);
    });
});
