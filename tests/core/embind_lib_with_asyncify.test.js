Module.onRuntimeInitialized = async () => {
  try {
    let fooResult = Module.foo();
    assert(fooResult instanceof Promise);
    fooResult = await fooResult;
    assert(fooResult === 10);

    let barInstancePromise = new Module.Bar();
    assert(barInstancePromise instanceof Promise);
    let barInstance = await barInstancePromise;
    assert(barInstance instanceof Module.Bar);
    assert(barInstance.x === 20);

    let barMethodResult = barInstance.method();
    assert(barMethodResult instanceof Promise);
    assert(await barMethodResult === 30);

    assert(barInstance.method_without_async_calls() === 40);

    let barStaticMethodResult = Module.Bar.static_method();
    assert(barStaticMethodResult instanceof Promise);
    assert(await barStaticMethodResult === 50);

    let err = '';
    try {
      barInstance.method();
      barInstance.method();
    } catch (e) {
      err = e.message;
    }
    assert(err.startsWith('abort(Assertion failed: Cannot have multiple async operations in flight at once)'));

    console.log('ok');
  } catch (e) {
    console.log('error: ' + e);
  }
};
