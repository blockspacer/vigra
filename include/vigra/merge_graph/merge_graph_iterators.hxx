    
#ifndef VIGRA_MERGE_GRAPH_ITERATORS_HXX
#define VIGRA_MERGE_GRAPH_ITERATORS_HXX

/* std library */
#include <set>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <deque>
#include <map>
#include <stdexcept>
#include <sstream>

/* boost */
#include <boost/function.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/iterator/filter_iterator.hpp>

/* vigra */
#include <vigra/multi_array.hxx>
#include <vigra/tinyvector.hxx>
#include <vigra/multi_array.hxx>
#include <vigra/graphs.hxx>

/* this project*/
#include "merge_graph_edge.hxx"
#include "merge_graph_node.hxx"
#include "merge_graph_item.hxx"
#include "merge_graph_callbacks.hxx"

/* this project (TO BE REFACTORED) */
#include "iterable_partition.hxx"
#include "macros.hxx"

namespace vigra {


// iterator functors
namespace merge_graph_detail{
    // iterator filters
    namespace filter{
        // functor to implement back neigh. iterator
        template<class ID_TYPE>
        struct SmallerThan{
            SmallerThan(){}
            SmallerThan(const ID_TYPE id): id_(id){}
            bool operator()(const ID_TYPE x) { return x < id_; }
            ID_TYPE id_;
        };
        // functor to implement back neigh.edge  iterator
        template<class MERGE_GRAPH>
        struct BackEdgeIdFilter{
            typedef MERGE_GRAPH MergeGraphType;
            typedef typename MergeGraphType::IdType IdType;
            BackEdgeIdFilter():graph_(NULL){}
            BackEdgeIdFilter(const MergeGraphType & graph, const IdType ownNodeId)
            :   graph_(&graph),
                ownNodeId_(ownNodeId){
            }
            // return if the other nodes id is smaller than own id
            bool operator()(const IdType edgeId)const{
                const IdType otherNodeid =  graph_->edgeFromId(edgeId).otherNodeId(ownNodeId_);
                return otherNodeid<ownNodeId_;
            }
            const MergeGraphType * graph_;
            IdType ownNodeId_;
        };
    }

    namespace transform{
        template<class MERGE_GRAPH,class GRAPH_ITEM>
        struct IdToGraphItem;

        template<class MERGE_GRAPH>
        struct IdToGraphItem<MERGE_GRAPH,typename MERGE_GRAPH::Edge>{
            IdToGraphItem():graph_(NULL){}
            IdToGraphItem(const MERGE_GRAPH & graph):graph_(&graph){}
            typename MERGE_GRAPH::Edge operator()(typename MERGE_GRAPH::IdType id)const{
                return graph_->edgeFromId(id);
            }
            const MERGE_GRAPH * graph_;
        };
        template<class MERGE_GRAPH>
        struct IdToGraphItem<MERGE_GRAPH,typename MERGE_GRAPH::Node>{
            IdToGraphItem():graph_(NULL){}
            IdToGraphItem(const MERGE_GRAPH & graph):graph_(&graph){}
            const typename MERGE_GRAPH::Node & operator()(typename MERGE_GRAPH::IdType id)const{
                return graph_->nodeFromId(id);
            }
            const MERGE_GRAPH * graph_;
        };
        // get other node id from edge id
        template<class MERGE_GRAPH>
        struct OtherNodeId{
            typedef MERGE_GRAPH MergeGraphType;
            typedef typename MergeGraphType::IdType IdType;
            OtherNodeId():graph_(NULL){}
            OtherNodeId(const MergeGraphType & graph, const IdType ownNodeId)
            :   graph_(&graph),
                ownNodeId_(ownNodeId){
            }
            // return the other node's is
            IdType operator()(const IdType edgeId)const{
                return graph_->edgeFromId(edgeId).otherNodeId(ownNodeId_);
            }
            const MergeGraphType * graph_;
            IdType ownNodeId_;
        };
        // get other node  from edge id
        template<class MERGE_GRAPH>
        struct OtherNode{
            typedef MERGE_GRAPH MergeGraphType;
            typedef typename MergeGraphType::IdType IdType;
            typedef typename MergeGraphType::Node   Node;
            OtherNode():graph_(NULL){}
            OtherNode(const MergeGraphType & graph, const IdType ownNodeId)
            :   graph_(&graph),
                ownNodeId_(ownNodeId){
            }
            // return the other node's is
            const Node & operator()(IdType edgeId)const{
                //CGP_ASSERT_OP(graph_,!=,NULL);
                const IdType otherNodeId= graph_->edgeFromId(edgeId).otherNodeId(ownNodeId_);
                return graph_->nodeFromId(otherNodeId);
            }
            const MergeGraphType * graph_;
            IdType ownNodeId_;
        };

    }




    template<class ITER>
    class IsEndIter
    :   public ITER {
    public:
        IsEndIter()
        :   ITER(),
            end_(),
            valid_(false){            
        }

        IsEndIter(ITER iter , ITER end)
        :   ITER(iter),
            end_(end),
            valid_(true){
        }

        bool isEnd()const{
            return !valid_ || *this==end_;
        }
    private:
        ITER end_;
        bool valid_;
    };

    template<class FILTTER,class ITER>
    class FilterIter
    :   public boost::filter_iterator< FILTTER,ITER>
    {
    public:
        FilterIter()
        :   boost::filter_iterator< FILTTER,ITER>(){
        }
        FilterIter(const FILTTER & filter, ITER  pos, ITER end)
        :   boost::filter_iterator< FILTTER,ITER>(filter,pos,end){
        }
        bool isEnd()const{
            //return *this == this->end();
            return this->base().isEnd();
        }

    };

    template<class TRANSFORM,class ITER,class REF,class VAL>
    class TransformIter
    :   public boost::transform_iterator< TRANSFORM,ITER,REF,VAL>
    {
    public:
        TransformIter()
        :boost::transform_iterator< TRANSFORM,ITER,REF,VAL>(){
        }
        TransformIter(ITER  pos ,const TRANSFORM & transform)
        :   boost::transform_iterator< TRANSFORM,ITER,REF,VAL>(pos,transform){
        }
        bool isEnd()const{
            return this->base().isEnd();
        }

    };


};


} // end namespace vigra



#endif //VIGRA_MERGE_GRAPH_ITERATORS_HXX