import os
import platform
import subprocess


file_path = os.path.dirname(os.path.abspath(__file__))
path = os.path.join(file_path, ".\\nanopb\\generator")
# command = "cd " + path + "./nanopb/generator && ./protoc '--nanopb_out=../../../lib/proto -f ../../../proto/dagr.options:../../../lib/proto' --proto_path=../../../proto ..\..\..\proto\dagr.proto"
# print(command)

test = ["cd", path, "&& ./protoc '--nanopb_out=../../../lib/proto -f ../../../proto/dagr.options:../../../lib/proto' --proto_path=../../../proto ..\..\..\proto\dagr.proto"]

print(path)


os.chdir(path)

if(platform.system() == "Windows"):
    # os.system(".\\protoc \'--nanopb_out=..\\..\\..\\lib\\proto -f ..\\..\\..\\proto\\dagr.options:..\\..\\..\\lib\\proto \' --proto_path=..\\..\\..\\proto ..\\..\\..\\proto\\dagr.proto")
    p = subprocess.Popen(["./protoc", "\'--nanopb_out=..\\..\\..\\lib\\proto -f ..\\..\\..\\proto\\dagr.options:..\\..\\..\\lib\\proto \'", "--proto_path=..\\..\\..\\proto ..\\..\\..\\proto\\dagr.proto"])
    p.wait()