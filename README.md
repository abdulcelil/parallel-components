# About the C++ Parallel Components Thread Container Library
With this library, communication between threads is simpler than global variables can be provided.
These components, represented by the node class, can transfer the results they produce to other components.
We can imagine these components in real life just like the units working in a factory or like humans.
These units can transfer what they produce to each other.The components consist of a simple Node class.
Node classes run the functions of the threads they are connected to and the values they produce
transfer them to the output queues they host.


# General Summary English
This code is  an implementation of a parallel components thread container library. It defines a templated Input class that takes a function object and runs it in a loop, producing results that are pushed onto a queue. The Input class also has a start() method that sets a flag to begin processing and a join() method that waits for the thread to finish execution.

The main() function initializes several Node objects, each taking input from another Node or the Input class. The Node objects perform some operation on the input, produce output, and pass that output to the next Node in the pipeline.

In the main loop of main(), the f4 object is used to retrieve the output produced by the pipeline, which is then printed to the console.

Overall, this code appears to be a basic implementation of a parallel pipeline processing library. However, there are some issues that need to be addressed to make it production-ready, such as error handling, more robust synchronization mechanisms, and handling of unexpected inputs or outputs.

This is a C++ implementation of a parallel processing library using a queue-based data flow model.

The library provides a Node class template that represents a component in a data processing pipeline. Each Node object is associated with a function that processes data and produces output data, and it can be connected to one or more input Node objects and one or more output Node objects.

The library uses the std::thread class to run each Node object in a separate thread, and it uses the std::mutex class to synchronize the access to the input and output queues. The library also provides several helper functions for managing the input and output queues.

The code defines a class template Node that represents a processing component. A Node object is created by providing a function object that defines the processing logic for the component. The Node class template has several member functions that allow it to communicate with other Node objects in the processing pipeline.

The main() function initializes several Node objects, each taking input from another Node or the Input class. The Node objects perform some operation on the input, produce output, and pass that output to the next Node in the pipeline.

Overall, this code provides a basic implementation of a parallel pipeline processing library. However, there are some issues that need to be addressed to make it production-ready, such as error handling, more robust synchronization mechanisms, and handling of unexpected inputs or outputs.

# Turkish
Bu kod, sıra tabanlı bir veri akış modeli kullanarak paralel işlemeli bir kütüphane için C++ uygulamasıdır.

Kütüphane, veri işleme boru hattındaki bir bileşeni temsil eden Node sınıf şablonunu sağlar. Her Node nesnesi, verileri işleyen ve çıktı verileri üreten bir işlevle ilişkilendirilir ve bir veya daha fazla giriş Node nesnesi ve bir veya daha fazla çıkış Node nesnesine bağlanabilir.

Kütüphane, her Node nesnesini ayrı bir iş parçacığında çalıştırmak için std::thread sınıfını kullanır ve giriş ve çıkış kuyruklarına erişimi senkronize etmek için std::mutex sınıfını kullanır. Kütüphane, giriş ve çıkış kuyruklarını yönetmek için birkaç yardımcı işlev de sağlar.

Kod, bir işleme bileşenini temsil eden sınıf şablonu Node'u tanımlar. Bir işlem mantığını tanımlayan bir işlev nesnesi sağlayarak bir Node nesnesi oluşturulur. Node sınıf şablonu, işlem hattındaki diğer Node nesneleriyle iletişim kurmasına izin veren birkaç üye işlevi içerir.

Main() işlevi, Input sınıfından veya başka bir Node'dan girdi alan birkaç Node nesnesini başlatır. Node nesneleri girdide bir işlem yapar, çıktı üretir ve bu çıktıyı boru hattındaki bir sonraki Node'a aktarır.

Bu kod, temel bir paralel boru hattı işlemeli kütüphane uygulaması sağlar. Ancak, hataların işlenmesi, daha sağlam senkronizasyon mekanizmaları ve beklenmeyen girdi veya çıktıların yönetimi gibi bazı konulara dikkat edilmesi gerekmektedir.

# Tests

I haven't written the test codes yet, I ran it in Visual Studio 2019.

# About Project
This project is experimental yet. It needs improvement. If you are interested, you can contribute. Thank you.
