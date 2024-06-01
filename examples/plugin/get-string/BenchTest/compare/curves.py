import os
import matplotlib.pyplot as plt
import math
import time
import random
import string

loop = 1e7
sizeb = 3
def random_string(length):
    letters = string.ascii_letters
    return ''.join(random.choice(letters) for i in range(length))

def fig_cpp():
    data = [[], []]
    for i in range(100):
        sizea = 10 + 10 * i
        string1=random_string(sizea)
        index=  sizea - sizeb-2
        string2 = random_string(200)

        result = os.popen("./index " + string1 + " " + str(loop)).read()
        data[0].append(sizea)
        data[1].append((float(result) / 1000))

    plt.figure("C++")
    plt.plot(data[0], data[1])

    plt.xlabel('string size')
    plt.ylabel('cost time (ms)')
    plt.ylim(0, 1000)
    plt.show()

def fig_js():
    data = [[], []]
    for i in range(100):
        sizea = 10 + 10 * i
        string1=random_string(1024)
        index=  sizea - sizeb-2
        string2 = string1[index: index + sizeb]

        result = os.popen("node index.js " + string1 + " " + string2 + " " + str(loop)).read()
        data[0].append((sizea))
        data[1].append(float(result))

    plt.figure("JavaScript")
    plt.plot(data[0], data[1])

    plt.xlabel('string size')
    plt.ylabel('cost time (ms)')
    plt.ylim(0, 1000)

    # plt.savefig('JavaScript.png')
    plt.show()


def fig_wasm():
    data = [[], []]
    for i in range(100):
        sizea = 10 + 10 * i
        string1=random_string(sizea)
        index=  sizea - sizeb-2
        string2 = random_string(200)

        result = os.popen(f"""/Users/junjie.zhu/Desktop/Code/WasmEdge/build/tools/wasmedge/wasmedge --string1="{string1}" --string2="{string2}" --upper=1 slices.wasm

        """
        ).read()
        data[0].append((sizea))
        # print(result)
        data[1].append(float(result.split('\n')[0])/1000)

    plt.figure("runtimecpp_concat")
    plt.plot(data[0], data[1])

    plt.xlabel('string size')
    plt.ylabel('cost time (ms)')
    plt.ylim(0, 1000)

    # plt.savefig('JavaScript.png')
    plt.show()

if __name__ == '__main__':
    fig_wasm()