#set BASEDIR to parent directory of forest, fire, geometry etc directories
BASEDIR = .

CPPFLAGS += -I$(BASEDIR)/geometry
CPPFLAGS += -I$(BASEDIR)/forest
CPPFLAGS += -I$(BASEDIR)/fire
CPPFLAGS += -I$(BASEDIR)/settings
CPPFLAGS += -I$(BASEDIR)/numerics
CPPFLAGS += -I$(BASEDIR)/io
CPPFLAGS += -I$(BASEDIR)/util

#CXX = /sfw/gcc/4.7.1/bin/g++-4.7
#CXX = /sfw/gcc/4.7.2/bin/x86_64-apple-darwin11.4.2-g++
CXX = g++ #ubuntu
#CXX = ~/Documents/FFM/Model/Code/02-06-13/mingw-w64-bin_i686-linux_20130523/bin/x86_64-w64-mingw32-g++
#CXX = /sfw/mingw_64/mingw-w64-bin_i686-darwin_20121031/bin/x86_64-w64-mingw32-c++ ##for win64
#CXX = /usr/bin/i686-w64-mingw32-g++
#CXX = clang++

CXXFLAGS += -std=c++11
CXXFLAGS += -g 
#CXXFLAGS += -fno-inline-small-functions #no optimisation for debugging
#CXXFLAGS += -O3

#for Win64
#CXXFLAGS += -static-libgcc -static-libstdc++

NUMERICS_HEADERS = $(BASEDIR)/numerics/*.h
GEOMETRY_HEADERS = $(BASEDIR)/geometry/*.h 
FOREST_HEADERS = $(BASEDIR)/forest/*.h
FIRE_HEADERS = $(BASEDIR)/fire/*.h
SETTINGS_HEADERS = $(BASEDIR)/settings/*.h

IO_HEADERS = $(BASEDIR)/io/*.h
UTIL_HEADERS = $(BASEDIR)/util/*.h

ALL_HEADERS += $(NUMERICS_HEADERS) 
ALL_HEADERS += $(GEOMETRY_HEADERS) 
ALL_HEADERS += $(FOREST_HEADERS) 
ALL_HEADERS += $(FIRE_HEADERS) 
ALL_HEADERS += $(SETTINGS_HEADERS) 
ALL_HEADERS += $(IO_HEADERS) 
ALL_HEADERS += $(UTIL_HEADERS) 

ffm : test.o location.o forest.o stratum.o ray.o line.o seg.o poly.o ffm_io.o flame.o \
	ffm_util.o ignition_path.o forest_ignition_run.o ffm_numerics.o
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@
location.o : $(BASEDIR)/forest/location.cc $(ALL_HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(BASEDIR)/forest/location.cc
ray.o : $(BASEDIR)/geometry/ray.cc $(ALL_HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(BASEDIR)/geometry/ray.cc
line.o : $(BASEDIR)/geometry/line.cc $(ALL_HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(BASEDIR)/geometry/line.cc
seg.o : $(BASEDIR)/geometry/seg.cc $(ALL_HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(BASEDIR)/geometry/seg.cc
poly.o : $(BASEDIR)/geometry/poly.cc $(ALL_HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(BASEDIR)/geometry/poly.cc
forest.o : $(BASEDIR)/forest/forest.cc $(ALL_HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(BASEDIR)/forest/forest.cc
stratum.o : $(BASEDIR)/forest/stratum.cc $(ALL_HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(BASEDIR)/forest/stratum.cc
flame.o : $(BASEDIR)/fire/flame.cc $(ALL_HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(BASEDIR)/fire/flame.cc
ignition_path.o : $(BASEDIR)/fire/ignition_path.cc $(ALL_HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(BASEDIR)/fire/ignition_path.cc
forest_ignition_run.o : $(BASEDIR)/forest/forest_ignition_run.cc $(ALL_HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(BASEDIR)/forest/forest_ignition_run.cc
ffm_io.o : $(BASEDIR)/io/ffm_io.cc $(ALL_HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(BASEDIR)/io/ffm_io.cc
ffm_util.o : $(BASEDIR)/util/ffm_util.cc $(ALL_HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(BASEDIR)/util/ffm_util.cc
ffm_numerics.o : $(BASEDIR)/numerics/ffm_numerics.cc $(ALL_HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(BASEDIR)/numerics/ffm_numerics.cc
test.o : $(BASEDIR)/forest/test.cc $(ALL_HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(BASEDIR)/forest/test.cc

clean :
	del *.o ffm.exe

