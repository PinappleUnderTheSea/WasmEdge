// 生成随机字符串
function generateRandomString(length) {
    let result = '';
    const characters = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789';
    const charactersLength = characters.length;
    for (let i = 0; i < length; i++) {
        result += characters.charAt(Math.floor(Math.random() * charactersLength));
    }
    return result;
}

// 计算算法的总运行时间

function test(i, str1, str2){
    return str1[0]
}
function calculateTotalRuntime() {
    const iterations = process.argv[4] ;
    let totalTime = 0;
    var startTime = performance.now();

    for (let i = 0; i < iterations; i++) {
        // const smallString = 'ebao'; // 生成长度为10的小字符串

        // const largeString = 'ouaimibaobaomibaobaoaibuaiouousuebaobao'; // 生成长度为1000的大字符串
        const smallString = process.argv[3]; // 生成长度为10的小字符串
        const largeString = process.argv[2]; // 生成长度为1000的大字符串
        // var startTime = performance.now();
        // console.log('Small string:', smallString);
        // console.log('Large string:', largeString);


        test(i, largeString, smallString)
        // console.log('Index:', index);


        // var runtime = endTime - startTime;
        // totalTime += runtime;
    }
    var endTime = performance.now();
    totalTime = endTime - startTime;
    console.log(totalTime);
}

calculateTotalRuntime();