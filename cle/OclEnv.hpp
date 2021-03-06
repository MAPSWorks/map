/**
 * @file	OclEnv.hpp
 * @author	Jesús Carabaño Bravo <jcaraban@abo.fi>
 *
 * TODO: OclEnv is to be redesigned
 * Currently its interface is good, but the internal implementation should change
 * e.g. using a parser to easily allow more complex expressions
 * e.g. using higher level structs like std::string since performance is not an issue here
 *
 * TODO: father could be changed from ptr to ref
 * TODO: OclEnv should have another 'underscore' class_, otherwise copies are dangerous
 */

#ifndef _OPENCL_ENVIRONMENT_HPP_
#define _OPENCL_ENVIRONMENT_HPP_

#define CL_USE_DEPRECATED_OPENCL_2_0_APIS

#ifdef __APPLE__
#	include <OpenCL/cl.h>
#elif __WIN32__
#	include <CL/cl.h>
#elif __GNUC__
#	include <CL/cl.h>
#endif

#include <vector>
#include <list>
#include <string>
#include <memory>
#include <mutex>
#include <cstdarg>
#include <cassert>


namespace cle {

#define clCheckError(error) checkError(error,__FILE__,__func__,__LINE__)

#define MAX_PLATFORMS 8
#define MAX_DEVICES 16
#define BUF_SIZE 1024

typedef unsigned int uint;

/***********
   Classes
 ***********/

class OpenclEnvironment;
struct Platform_;
class Platform;
struct Device_;
class Device;
struct Context_;
class Context;
struct Task_;
class Task;
struct Kernel_;
class Kernel;
struct Queue_;
class Queue;

typedef OpenclEnvironment OclEnv;

struct Trace {
	int platform, device, context, task, kernel, queue;
	std::string task_name;
	Trace() : platform(-1), device(-1), context(-1), task(-1), kernel(-1), queue(-1) { }
};

/**********************
   OpenCL Environment
 **********************/

class OpenclEnvironment {
	friend class Platform;
	friend class Device;
	friend class Context;
	friend class Task;
	friend class Kernel;
	friend class Queue;

  private:
	std::vector<Platform_> vPlatform;
	std::vector<Device_> vDevice;
	std::vector<Context_> vContext;
	std::vector<Task_> vTask;
	std::vector<Kernel_> vKernel;
	std::vector<Queue_> vQueue;
	std::shared_ptr<std::mutex> m;

	int parse(const char* str, Trace trace, va_list args);
	bool evaluateConditions(cl_platform_id id, char* str, va_list args);

	int addDevice(cl_platform_id pid, cl_device_id id);
	int addContext(cl_platform_id pid, cl_device_id* did, cl_uint ndev, cl_context id);
	int addTask(cl_context cid, cl_program id);
	int addKernel(cl_program pid, cl_kernel id);
	int addQueue(cl_context cid, cl_device_id did, cl_command_queue id);

	int removeDevice(cl_platform_id pid, cl_device_id id);

  public:
	OpenclEnvironment();
	~OpenclEnvironment();
	OpenclEnvironment(const OpenclEnvironment& other) = delete; // Copy constructor: disabled
	const OpenclEnvironment& operator=(const OpenclEnvironment& other) = delete; // Assignment operator: disabled

	void clear(); // All P/D/C/T/K/Q objects become invalid after clear()
	int init(const char* str...);

	int addPlatform(cl_platform_id id);

	// Accessors
	Platform platform(int i);
	int platformSize();
	Device device(int i);
	int deviceSize();
	Context context(int i);
	int contextSize();
	Task task(int i);
	int taskSize();
	Kernel kernel(int i);
	int kernelSize();
	Queue queue(int i);
	int queueSize();

	// Shortcuts
	Platform P(int i);
	int nP();
	Device D(int i);
	int nD();
	Context C(int i);
	int nC();
	Task T(int i);
	int nT();
	Kernel K(int i);
	int nK();
	Queue Q(int i);
	int nQ();
};

/************
   Platform
 ************/

struct Platform_ {
	std::vector<int> vrDevice;
	std::vector<int> vrContext;
	cl_platform_id sId;
};

class Platform {
	friend class OpenclEnvironment;

  private:
	OpenclEnvironment *father;
	int ref;

	Platform(OpenclEnvironment *father, int plt);

	int parse(const char* str, Trace trace, va_list args);
	bool evaluateConditions(cl_device_id id, char* cond, va_list args);

  public:
  	Platform();
	// Copy constructor: autogenerated
	// Assignment operator: autogenerated

	int init(const char* str...);
	
  	int addDevice(cl_device_id id);
	int addContext(cl_device_id* did, cl_uint ndev, cl_context id);

	int removeDevice(cl_device_id id);

	const cl_platform_id& id() const;
	const cl_platform_id& operator*() const;
	const void* get(cl_platform_info info) const;

	// Accessors
	const OpenclEnvironment& environment() const;
	OpenclEnvironment& environment();
	Device device(int i);
	int deviceSize();
	Context context(int i);
	int contextSize();

	// Shortcuts
	Device D(int i);
	int nD();
	Context C(int i);
	int nC();
};

/**********
   Device
 **********/

struct Device_ {
	int rPlatform;
	std::vector<int> vrContext;
	std::vector<int> vrQueue;
	cl_device_id sId;
};

class Device {
	friend class OpenclEnvironment;
	friend class Platform;

  private:
	OpenclEnvironment *father;
	int ref;

	Device(OpenclEnvironment *father, int dev);

	int parse(const char* str, Trace trace, va_list args);
	bool evaluateConditions(cl_device_id id, char* str, va_list args);
	
  public:
  	Device();
	// Copy constructor: autogenerated
	// Assignment operator: autogenerated

	int init(const char* str...);

	int addContext(cl_context id);
	int addQueue(cl_context cid, cl_command_queue id);

	const cl_device_id& id() const;
	const cl_device_id& operator*() const;
	const void* get(cl_device_info info) const;

	// Accessors
	const OpenclEnvironment& environment() const;
	OpenclEnvironment& environment();
    Platform platform();
	Context context(int i);
	int contextSize();
	Queue queue(int i);
	int queueSize();

	// Shortcuts
	Platform P();
	Context C(int i);
	int nC();
	Queue Q(int i);
	int nQ();
};

/***********
   Context
 ***********/

struct Context_ {
	int rPlatform;
	std::vector<int> vrDevice;
	std::vector<int> vrTask;
	std::vector<int> vrQueue;
	cl_context sId;
};

class Context {
	friend class OpenclEnvironment;
	friend class Device;

  private:
	OpenclEnvironment *father;
	int ref;

	Context(OpenclEnvironment *father, int ctx);
	
	int parse(const char* str, Trace trace, va_list args);

  public:
  	Context();
  	// Copy constructor: autogenerated
	// Assignment operator: autogenerated

	int init(const char* str...);
		
	int addTask(cl_program id);
	int addQueue(cl_device_id did, cl_command_queue id);

	const cl_context& id() const;
	const cl_context& operator*() const;
	
	// Accessors
	const OpenclEnvironment& environment() const;
	OpenclEnvironment& environment();
	Platform platform();
	Device device(int i);
	int deviceSize();
	Task task(int i);
	int taskSize();
	Queue queue(int i);
	int queueSize();

	// Shortcuts
	Platform P();
	Device D(int i);
	int nD();
	Task T(int i);
	int nT();
	Queue Q(int i);
	int nQ();
};

/***********
   Task
 ***********/

struct Task_ {
	int rContext;
	std::vector<int> vrKernel;
	cl_program sId;
};

class Task {
	friend class OpenclEnvironment;
	friend class Context;

  private:
	OpenclEnvironment *father;
	int ref;
	
	Task(OpenclEnvironment *father, int tsk);

	int parse(const char* str, Trace trace, va_list args);

  public:
  	Task();
  	// Copy constructor: autogenerated
	// Assignment operator: autogenerated

	int init(const char* str...);

	int addKernel(cl_kernel id);

	const cl_program& id() const;
	const cl_program& operator*() const;

	const OpenclEnvironment& environment() const;
	OpenclEnvironment& environment();
	Context context();
	Kernel kernel(int i);
	int kernelSize();

	/*
	DEFINE_ENVIRONMENT_SHORTCUT_1
	DEFINE_CONTEXT_SHORTCUT_1
	DEFINE_KERNEL_SHORTCUT_X
	*/
	Context C();
	Kernel K(int i);
	int nK();
};

/**********
   Kernel
 **********/

struct Kernel_ {
	int rTask;
	cl_kernel sId;
};

class Kernel {
	friend class OpenclEnvironment;
	friend class Task;

  private:
	OpenclEnvironment *father;
	int ref;

  	Kernel(OpenclEnvironment *father, int krn);
  	
  	int parse(const char* str, Trace trace, va_list args);
	
  public:
  	Kernel();
  	// Copy constructor: autogenerated
	// Assignment operator: autogenerated

	const cl_kernel& id() const;
	const cl_kernel& operator*() const;

	const OpenclEnvironment& environment() const;
	OpenclEnvironment& environment();
	Task task();

	/*
	DEFINE_ENVIRONMENT_SHORTCUT_1
	DEFINE_TASK_SHORTCUT_1
	*/
	Task T();
};

/*********
   Queue
 *********/

struct Queue_ {
	int rDevice;
	int rContext;
	cl_command_queue sId;
};

class Queue {
	friend class OpenclEnvironment;

  private:
	OpenclEnvironment *father;
	int ref;

	Queue(OpenclEnvironment *father, int que);
	
  public:
	Queue();
	// Copy constructor: autogenerated
	// Assignment operator: autogenerated
	
	const cl_command_queue& id() const;
  	const cl_command_queue& operator*() const;

	const OpenclEnvironment& environment() const;
	OpenclEnvironment& environment();
	Device device();
	Context context();

	/*
	DEFINE_ENVIRONMENT_SHORTCUT_1
	DEFINE_DEVICE_SHORTCUT_1
	DEFINE_CONTEXT_SHORTCUT_1
	*/
	Device D();
	Context C();
};

/*********
   Utils
 *********/

void checkError(cl_int error, const char* file, const char* func, int line);

cl_platform_info str_to_platinfo(const char* str);
cl_device_info str_to_devinfo(const char* str);
const char * error_to_str(cl_int err);

} // namespace cle

#endif
