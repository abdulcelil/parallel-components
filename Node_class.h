/* ---------------------------------------------------------------------
 * Parallel Components Thread Container Library
 * Copyright (C) 2022 ,Abdulcelil DOÐAN.  Unless you have an agreement
 * with Abdulcelil DOÐAN, for a separate license for this software code, the
 * following terms and conditions apply:
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Affero Public License for more details.
 *
 * You should have received a copy of the GNU Affero Public License
 * along with this program.  If not, see http://www.gnu.org/licenses.
 *
 * https://abdulcelil.com.tr/
 * abdulcelildogan@gmail.com
 * ----------------------------------------------------------------------
 */
#pragma once
#include <functional>
#include <iostream>
#include <thread>
#include <queue>
#include <tuple>
#include <utility>
#include <vector>
#include <cstdlib>
#include <mutex>
#include <type_traits>
#include <string>
#include <algorithm>

 //This is the global mutex for all components
std::mutex mutex{};
//Test auxiliary function to check whether all queues at the output of a component are equal in size.
//The purpose of this function is to distribute all output queues equally.
template<typename T>
bool queue_equality(const std::vector<std::queue<T>>&producer_output_queues)
{
    for (int i = 0; i <producer_output_queues.size() - 1; i++)
    {
        if (producer_output_queues.at(i).size() != producer_output_queues.at(i + 1).size())
            return false;
    }
    return true;
}



//Allows the components to be packaged ,to be prepared for receiving their packages.
template <class Tuple, std::size_t... I>
void send_ready_to_all_producers(Tuple& t, std::index_sequence<I...>)
{

    (std::get<I>(t).producer_get_order(I) && ...);

}

template <typename ReturnType, typename... FunctionParamTypes>
class Node
{
private:

    //The number of subsequently started components that the component will pack
    int consumer_number{};

    //thread start flag ,default true 
    bool start_flag{ true };

    //vector to hold the thread, threads can only be kept in this way, because otherwise it must be defined beforehand
    std::thread thread;

    //lambda function object that threads will run
    std::function<ReturnType(FunctionParamTypes...)> function;

    //determines in which queue the packet will be delivered
    int prod_queue_order_to_send{ 0 };

    //output queues of the component
    std::vector<std::queue<ReturnType>>producer_output_queues;

    bool ready_flag{ false };
public:



    //constructor
    Node<ReturnType, FunctionParamTypes...>(const std::function<ReturnType(FunctionParamTypes...)>& function_)
        :function(function_)
    {

    }

    //Thread function
    //Takes as parameters the component objects whose packages it will receive
    template <typename... ProducerNodes>
    void run(ProducerNodes& ...producer_nodes)
    {
        while (start_flag)
        {
          

            
            
            if (ready_flag)
            {

                mutex.lock();

                //To prevent simultaneous reception of other components

                using Tuple = std::tuple<ProducerNodes &...>;
                Tuple tuple(producer_nodes...);

                constexpr auto size = std::tuple_size_v<std::remove_reference_t<Tuple>>;
                auto mki = std::make_index_sequence<size>{};

                //It is requested to prepare the components whose packages will be taken.
                send_ready_to_all_producers(tuple, mki);

                //If the output queues are empty or have equal sizes, start the reception.
                if (queue_equality<ReturnType>(producer_output_queues))
                {
                    //Start package recevive if other components are ready
                    if ((producer_nodes.producer_get_ready() && ...))
                    {
                        //Receive and process packages
                        ReturnType  r = function(producer_nodes.producer_get_packet() ...);

                        //Accept if lambda result is true
                        if (r.get())
                        {
                            //Push function result to output queues
                            for (std::queue<ReturnType>& q : producer_output_queues)
                            {
                                q.push(r);
                            }
                            //Data received and other components stop sending until next reception
                            ((producer_nodes.producer_stop() && ...));


                        }
                    }
                }

                mutex.unlock();
            }
           
            
        }


    }

    bool consumer_number_inc()
    {
        ++consumer_number;
        producer_output_queues.push_back(std::queue<ReturnType>{});

        return true;
    }

    //Queue number to start the sends
    bool producer_get_order(int order)
    {

        prod_queue_order_to_send = order;
        return true;

    }

    //Cancel package delivery until next pick up
    bool producer_stop()
    {

        prod_queue_order_to_send = -1;


        return true;

    }

    //send package from requested queue
    ReturnType producer_get_packet()
    {

        if (producer_get_ready())
        {
            ReturnType res =producer_output_queues.at(prod_queue_order_to_send).front();
            producer_output_queues.at(prod_queue_order_to_send).pop();
            return res;
        }
        return ReturnType();
    }

    // Is the package ready to send?
    bool producer_get_ready() const
    {
        if (prod_queue_order_to_send > -1 && 
            prod_queue_order_to_send <= consumer_number - 1 &&
            prod_queue_order_to_send <producer_output_queues.size() &&
            !producer_output_queues.at(prod_queue_order_to_send).empty())
        {
            return true;
        }

        return false;

    }


    //the function that will trigger the thread and pass the components to the run function
    template <typename... ProducerNodes>
    void handle(ProducerNodes& ...producer_nodes) //Thread başlatılmayacak , uyku moduna alınacak
    {

        //Consumer sayısını artır
       ((producer_nodes.consumer_number_inc() && ...));


        thread=std::thread(&Node<ReturnType, FunctionParamTypes...>::run<ProducerNodes&...>, this, std::ref(producer_nodes) ...);
    }
 
    
    void start()
    {
        
        ready_flag = true;

    }
    
    void join()
    {
        if (thread.joinable())
            thread.join();
    }
};

