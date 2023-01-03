# About the C++ Parallel Components Thread Container Library
With this library, communication between threads is simpler than global variables can be provided.
These components, represented by the node class, can transfer the results they produce to other components.
We can imagine these components in real life just like the units working in a factory or like humans.
These units can transfer what they produce to each other.The components consist of a simple Node class.
Node classes run the functions of the threads they are connected to and the values they produce
transfer them to the output queues they host.


# Components are defined as

The constructor must contain a connection number integer and a lambda expression.
Connection number is the component number that receives results from this component.
connection numbers will be automated in my next work.
For the Node Components template parameters, the first parameter is the data type in the outputs, 
the other parameters are the data types of the parameters of the lambda expression

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
