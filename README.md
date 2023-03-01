# About the C++ Parallel Components Thread Container Library
With this library, communication between threads is simpler than global variables can be provided.
These components, represented by the node class, can transfer the results they produce to other components.
We can imagine these components in real life just like the units working in a factory or like humans.
These units can transfer what they produce to each other.The components consist of a simple Node class.
Node classes run the functions of the threads they are connected to and the values they produce
transfer them to the output queues they host.


# Creating components

The constructor must contain a connection number integer and a lambda expression.
Connection number is the component number that receives results from this component.
connection numbers will be automated in my next work.
For the Node Components template parameters, the first parameter is the data type in the outputs, 
the other parameters are the data types of the parameters of the lambda expression

# General Summary
This is a C++ header file that defines a library for creating threaded components that communicate through queues. The library is called Parallel Components Thread Container Library and is owned by Abdulcelil DOGAN.
The header file starts by defining the terms and conditions under which the program can be used, modified, and distributed. It specifies that the program is free software that can be redistributed or modified under the GNU Affero Public License version 3. The author provides no warranty for the program.

The header file defines a global mutex that can be used by all components in the library. It then defines a function queue_equality that checks if all output queues of a component have the same size.

The header file also defines a template class called Node that represents a threaded component. The class has a constructor that takes a lambda function object as an argument. The lambda function object represents the processing logic of the component. The class has a run method that takes as parameters the components whose packages it will receive. The method receives and processes the packages and then sends the results to the output queues of the component.

The Node class has several member variables that store information about the component. These include consumer_number, which is the number of subsequently started components that the component will pack, start_flag, which is a flag that indicates whether the component is running, function, which is the lambda function object that represents the processing logic of the component, prod_queue_order_to_send, which is an integer that determines in which queue the packet will be delivered, and producer_output_queues, which is a vector that stores the output queues of the component.

The Node class also has several member functions that perform various tasks. These include consumer_number_inc, which increments the number of subsequently started components that the component will pack and adds an output queue to the component, producer_get_order, which determines the order in which the component will send its packages, producer_stop, which cancels package delivery until the next pick-up, producer_get_packet, which sends a package from the requested queue, producer_get_ready, which checks if the package is ready to send, and send_ready_to_all_producers, which allows the components to be packaged, to be prepared for receiving their packages.

This code defines a library for implementing a parallel components thread container. The library provides a Node class template that represents a component in a data processing pipeline. Each Node object is associated with a function that processes data and produces output data, and it can be connected to one or more input Node objects and one or more output Node objects.

The library uses a queue-based data flow model to process data. Each Node object has one or more input queues and one or more output queues, and it receives input data from its input queues and produces output data to its output queues. The input and output queues are implemented using the std::queue standard library container.

The library uses the std::thread class to run each Node object in a separate thread, and it uses the std::mutex class to synchronize the access to the input and output queues. The library also provides several helper functions for managing the input and output queues, including queue_equality function that checks whether all output queues of a component have equal sizes, and send_ready_to_all_producers function that sends a signal to all input Node objects indicating that the component is ready to receive input data.

The library is licensed under the GNU Affero Public License version 3, which is a copyleft license that requires all modifications and derivative works to be licensed under the same license.

This is a C++ source code implementing a thread container library. The library allows users to define components and link them together to form a processing pipeline. Each component can receive inputs from one or more upstream components and produce outputs to one or more downstream components.

The code defines a class template Node that represents a processing component. A Node object is created by providing a function object that defines the processing logic for the component. The function object takes one or more arguments of a specific type and returns a value of a specific type.

The Node class template has several member functions that allow it to communicate with other Node objects in the processing pipeline. The run function starts a new thread and waits for inputs from upstream components. The consumer_number_inc function is called by downstream components to register the Node object as a producer of output. The producer_get_order function is called by upstream components to specify the output queue to which a produced value should be sent. The producer_get_packet function is called by downstream components to retrieve the next available output value.

The code also includes some helper functions. The queue_equality function checks whether all queues at the output of a component are equal in size. The send_ready_to_all_producers function prepares components for receiving their inputs. Finally, the code defines a global mutex mutex that is used to ensure thread safety.

This code has also  an implementation of a parallel components thread container library. It defines a templated Input class that takes a function object and runs it in a loop, producing results that are pushed onto a queue. The Input class also has a start() method that sets a flag to begin processing and a join() method that waits for the thread to finish execution.

The main() function initializes several Node objects, each taking input from another Node or the Input class. The Node objects perform some operation on the input, produce output, and pass that output to the next Node in the pipeline.

In the main loop of main(), the f4 object is used to retrieve the output produced by the pipeline, which is then printed to the console.

Overall, this code appears to be a basic implementation of a parallel pipeline processing library. However, there are some issues that need to be addressed to make it production-ready, such as error handling, more robust synchronization mechanisms, and handling of unexpected inputs or outputs.





```

// Defining data structure, data can be smart ponter
using StringPtr = std::shared_ptr<std::string>;


  Node<StringPtr, StringPtr> f1(2, [&](const StringPtr a) 
        {
            //Decides whether to process the value in its input
            if (a.get())
            {

                // Value to be transferred from this component to other components
                return  std::make_shared<std::string>("f1(" + *a + ")");
            }
            return StringPtr();
       
        });

```
# Use of Components:

```
 Node<StringPtr, StringPtr> comp1(2, [&](const StringPtr a)
        {
            if (a.get())
            {
				//.......

                return   .... // value to returned
            }
            return StringPtr();
         
        });

 Node<StringPtr, StringPtr> comp2(1, [&](const StringPtr a)
        {
            if (a.get())
            {
				//.......

                return   .... // value to returned
            }
            return StringPtr();
         
        });


    Node<StringPtr, StringPtr, StringPtr> comp3(1, [&](const StringPtr a, const StringPtr b)
        {
          
            if (a.get() && b.get())
            {
               //.......

                return   .... // 
            
            }
            return StringPtr();

        });
```

The use of components defined in this way is as follows,
In addition to these three components,
 there should be Input and Output component classes defined differently from the Node class.

```
// With the start function, it is defined from which components the component will receive the results.
Input input{};
Output output{};
comp1.start(input);
comp2.start(comp1);
comp3.start(comp1,comp2);
output.start(comp3);


// Wait for the processes to finish
input.join();
output.join();
comp1.join();
comp2.join();
comp3.join();
```
# Tests

I haven't written the test codes yet, I ran it in Visual Studio 2019.

# About Project
This project is experimental yet. It needs improvement. If you are interested, you can contribute. Thank you.
