# Le compilateur à utiliser
CXX = g++
# Les options du compilateur
CXXFLAGS = -mmacosx-version-min=10.12 -Wall -std=c++11 -g -I/u/usr/local/include/ -lglfw -lGLEW -framework OpenGL



quickAndDirtyIteration: quickAndDirtyIteration.cpp fileReader.o curve.o cameraControl.o fusionVectorSort.o
triTest: triTest.cpp fusionVectorSort.o


curve.o: curve.cpp fileReader.hpp curve.hpp
fileReader.o: fileReader.cpp fileReader.hpp
cameraControl.o: cameraControl.cpp cameraControl.hpp

fusionVectorSort.o: fusionVectorSort.cpp fusionVectorSort.hpp

clean: rm *.o