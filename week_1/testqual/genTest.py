import random
import subprocess

begin = ["char *", "const char *"]
lexems = ["", "const"]

# gen C type
def genCppType(begin, cont, n = 3):
    res = random.choice(begin)
    bracketsCount = random.choice([0, 1]) # for more no array checking
    for _ in range(n - bracketsCount):
        res += random.choice(cont) + "*"
    res += random.choice(cont)
    res += '[]' * bracketsCount
    return res

# gen C file 
def genTest(filename="test.cpp",  n = 3):
    cpp = """
    #include <iostream>
    #include <type_traits>
    int main() {
        std::cout << std::boolalpha << std::is_convertible_v<T1, T2> << std::endl;
        return 0;
    }
    """
    T1 = genCppType(begin, lexems, n)
    T2 = genCppType(begin, lexems, n)

    cpp = cpp.replace("T1", T1).replace("T2", T2)
    with open(filename, "w") as f:
        print(cpp, file=f)
    return T1, T2

def checkOut(output):
    convertible = None
    if "true" in output:
        convertible = True
    if "false" in output:
        convertible = False
    return convertible

# test compiler if stderr empty good conversion
def TestCompiler(filename):
    result = subprocess.run(['g++', filename], capture_output=True, text=True)
    error = result.stderr
    if len(error) > 0:
        return None, error
    result = subprocess.run(['./a.out'], capture_output=True, text=True)
    output = result.stdout
    return checkOut(output), output

# test function in output must be "true" if good conversion and args from argv[1] and argv[2]
def TestMy(filename, firstType, secondType):
    result = subprocess.run([filename, firstType, secondType], capture_output=True, text=True)
    output = result.stdout
    error = result.stderr
    if len(error) > 0:
        return None, error
    return checkOut(output), output

# test cycle stochastic generation with set length
for i in range(100):
    name = "test" + str(i) + ".cpp"
    T1, T2 = genTest(filename=name, n=2)
    my, myOut = TestMy("./testqual", T1, T2)
    compiler, compOut = TestCompiler(filename=name)
    if (my is None) or (compiler is None) or (my != compiler):
        print(i, " ", T1, " ||| ", T2)
        print("--------------------------------------", "MyOut:", myOut, "--------------------------------------", sep="\n")
        print("--------------------------------------", "CompilerOut:", compOut, "--------------------------------------", sep="\n")

