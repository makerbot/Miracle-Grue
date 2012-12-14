/* 
 * File:   pather_hierarchical_impl.h
 * Author: Dev
 *
 * Created on December 14, 2012, 12:06 PM
 */

#ifndef MGL_PATHER_HIERARCHICAL_IMPL_H
#define	MGL_PATHER_HIERARCHICAL_IMPL_H

#include "pather_hierarchical_decl.h"

namespace mgl {

template <typename LABEL_COMPARE, typename BOUNDARY_TEST>
void pather_hierarchical::InsetTree::traverse(LabeledOpenPaths& result, 
        Point2Type& entryPoint, const LABEL_COMPARE& labeler, 
        const BOUNDARY_TEST& bounder) {
    parent_class::iterator childIter = end();
    while(!empty()) {
        childIter = selectBestChild(entryPoint, labeler, bounder);
        if(labeler.compare(m_label, childIter->m_label) == BETTER) {
            //we want to do this node before all children worse than it
            break;
        }
    }
    //traverse self
    while(!empty()) {
        childIter = selectBestChild(entryPoint, labeler, bounder);
        childIter->traverse(result, entryPoint, labeler, bounder);
        erase(childIter);
    }
}
template <typename LABEL_COMPARE, typename BOUNDARY_TEST>
pather_hierarchical::InsetTree::parent_class ::iterator
        pather_hierarchical::InsetTree::selectBestChild(Point2Type& entryPoint, 
        const LABEL_COMPARE& labeler, const BOUNDARY_TEST& bounder) {
    //we basically do std::min_element, but store some data to avoid 
    //recomputing things like minimum distance and crossing test results
    parent_class::iterator bestIter = end();
    PathLabel bestLabel;
    bestLabel.myValue = std::numeric_limits<int>::min();
    Scalar bestSquaredDistance = std::numeric_limits<Scalar>::max();
    bool bestBounderResult = false;
    for(parent_class::iterator iter = begin(); 
            iter != end(); 
            ++iter) {
        InsetTree& currentChild = *iter;
        int labelResult = labeler.compare(bestLabel, currentChild.m_label);
        if(labelResult == WORSE) {
            //we don't even look at worse labels
            continue;
        } else if(labelResult == BETTER) {
            //discard all old data, this is our new best
            bestSquaredDistance = std::numeric_limits<Scalar>::max();
            bestBounderResult = false;
            bestLabel = currentChild.m_label;
        }
        Scalar squaredDistance = std::numeric_limits<Scalar>::max();
        bool bounderResult = false;
        for(Loop::const_finite_cw_iterator loopIter = 
                currentChild.boundary().clockwiseFinite(); 
                loopIter != currentChild.boundary().clockwiseEnd(); 
                ++loopIter) {
            Segment2Type testSegment = (entryPoint, *loopIter);
            Scalar sd = (entryPoint - *loopIter).squaredMagnitude();
            if(!bounderResult) {
                //no point has yet passed bounder
                if(bounder(testSegment)) {
                    //first point in this loop that passes bounder
                    bounderResult = true;
                    squaredDistance = sd;
                } else {
                    //no point has passed bounder, record shortest distance
                    if(sd < squaredDistance)
                        squaredDistance = sd;
                }
            } else {
                //another point passed bounder
                if(bounder(testSegment)) {
                    //we have a point that passed bounder, consider distances
                    if(sd < squaredDistance)
                        squaredDistance = sd;
                }
            }
        }
        if(!bestBounderResult) {
            //no loop has yet passed bounder
            if(bounderResult) {
                //first loop to pass bounder
                bestBounderResult = true;
                bestSquaredDistance = squaredDistance;
                bestIter = iter;
            } else {
                //no loop has passed bounder, get best distance
                if(squaredDistance < bestSquaredDistance) {
                    bestSquaredDistance = squaredDistance;
                    bestIter = iter;
                }
            }
        } else {
            //another loop passed bounder
            if(bounderResult) {
                //we have a loop that passed bounder, consider distances
                if(squaredDistance < bestSquaredDistance) {
                    bestSquaredDistance = squaredDistance;
                    bestIter = iter;
                }
            }
        }
    }
    return bestIter;
}
template <typename LABEL_COMPARE, typename BOUNDARY_TEST>
void pather_hierarchical::InsetTree::traverseInternal(
        LabeledOpenPaths& result, Point2Type& entryPoint, 
        const LABEL_COMPARE& labeler, 
        const BOUNDARY_TEST& bounder) {
    bool graphTraversed = false;
    //decide if we want to do graph before or after
    for(SpacialGraph::entry_iterator entryIter = m_graph.entryBegin(); 
            entryIter != m_graph.entryEnd(); 
            ++entryIter) {
        if(labeler(entryIter->data().label(), m_label) == BETTER) {
            //do graph before
            graphTraversed = true;
            m_graph.optimize(result, entryPoint, labeler, bounder);
            break;
        }
    }
    //optimize the inset
    //done optimizing the inset
    if(!graphTraversed) //graph wasn't done before, so do after
        m_graph.optimize(result, entryPoint, labeler, bounder);
}

}

#endif	/* MGL_PATHER_HIERARCHICAL_IMPL_H */

