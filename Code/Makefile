CXX = g++
CXXFLAGS = -std=c++11 -Wall -O3
 
SRCS = main.cpp ray.h sphere.h triangle.h vec3.h color.h cylinder.h hit_record.h image_writer.h material.h pinhole_camera.h point_light.h

OBJS = $(SRCS:.cc=.o)

TARGET = ray_tracer

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)