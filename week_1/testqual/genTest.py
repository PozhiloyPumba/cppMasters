import random
import subprocess

begin = ["char *", "const char *"]
lexems = ["", "const"]

# gen C type
def genCppType(begin, cont, n = 3):
    res = random.choice(begin)
    for _ in range(n):
        res += random.choice(cont) + "*"
    res += random.choice(cont)
    return res

# gen C file 
def genTest(filename="test.cpp",  n = 3):
    cpp = """
    int main() {
        T1 a = {};
        T2 b = a;
        return 0;
    }
    """
    T1 = genCppType(begin, lexems, n)
    T2 = genCppType(begin, lexems, n)

    cpp = cpp.replace("T1", T1).replace("T2", T2)
    with open(filename, "w") as f:
        print(cpp, file=f)
    return T1, T2

# test compiler if stderr empty good conversion
def TestCompiler(filename):
    result = subprocess.run(['g++', filename], capture_output=True, text=True)
    output = result.stdout
    error = result.stderr
    compiled = False
    if len(error) == 0:
        compiled = True
    return compiled

# test function in output must be "true" if good conversion and args from argv[1] and argv[2]
def TestMy(filename, firstType, secondType):
    result = subprocess.run([filename, firstType, secondType], capture_output=True, text=True)
    output = result.stdout
    error = result.stderr
    convertible = False
    if "true" in output:
        convertible = True
    return convertible

# test cycle stochastic generation with set length
for i in range(10):
    name = "test" + str(i) + ".cpp"
    T1, T2 = genTest(filename=name, n=1)
    if TestMy("./testqual", T1, T2) != TestCompiler(filename=name):
        print(T1, " ||| ", T2)

