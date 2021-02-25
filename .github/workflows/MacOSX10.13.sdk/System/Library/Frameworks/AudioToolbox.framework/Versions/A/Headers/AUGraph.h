/*!
	@file		AUGraph.h
	@framework	AudioToolbox.framework
	@copyright	(c) 2000-2015 by Apple, Inc., all rights reserved.
	@abstract	API's to manage graphs of AudioUnits.

	@discussion
		An AUGraph is a complete description of an audio signal processing network. The AUGraph
		API's maintain a set of AudioUnits, audio connections between their inputs and outputs, and
		any callbacks used to provide inputs. It also allows the embedding of sub-graphs within
		parent graphs to allow for a logical organization of parts of an overall signal chain. The
		graph's AudioUnits do the actual audio processing.

		The AUGraph may be introspected, in order to get complete information about all of the
		AudioUnits in the graph.  The various nodes (AUNode) in the AUGraph representing AudioUnits
		or sub graphs may be added or removed, and the interactions between them modified.

		An AUGraph's state can be manipulated in both the rendering thread and in other threads.
		Consequently, any activities that affect the state of the graph are guarded with locks and a
		messaging model between any calling thread and the thread upon which the AUGraph's output
		unit is called (the render thread).

		An AUGraph will have a single head node - what is referred to below as its output unit. The
		output unit is used to both start and stop the rendering operations of a graph, and as the
		dispatch point for the safe manipulation of the state of the graph while it is running.
*/

#ifndef AudioToolbox_AUGraph_h
#define AudioToolbox_AUGraph_h

#include <Availability.h>
#if !defined(__COREAUDIO_USE_FLAT_INCLUDES__)
	#include <CoreAudio/CoreAudioTypes.h>
	#include <AudioToolbox/AudioUnit.h>
#else
	#include <CoreAudioTypes.h>
	#include <AUComponent.h>
#endif

CF_ASSUME_NONNULL_BEGIN

#if defined(__cplusplus)
extern "C"
{
#endif

/*!
	@typedef	AUGraph
	@abstract	A reference to an AUGraph object.
*/
typedef	struct OpaqueAUGraph	*AUGraph;

/*!
	@typedef	AUNode
	@abstract	Used to represent a member of an AUGraph
*/
typedef SInt32 	AUNode;

/*!
    @enum AUGraph error codes
    @abstract   These are the error codes returned from the AUGraph API.
    @constant   kAUGraphErr_NodeNotFound
				The specified node cannot be found
	@constant	kAUGraphErr_InvalidConnection
				The attempted connection between two nodes cannot be made
	@constant	kAUGraphErr_OutputNodeErr
				AUGraphs can only contain one OutputUnit.
				this error is returned if trying to add a second output unit
				or the graph's output unit is removed while the graph is running
	@constant	kAUGraphErr_CannotDoInCurrentContext
				To avoid spinning or waiting in the render thread (a bad idea!), many of the
				calls to AUGraph can return: kAUGraphErr_CannotDoInCurrentContext. This
				result is only generated when you call an AUGraph API from its render
				callback. It means that the lock that it required was held at that time, by
				another thread. If you see this result code, you can generally attempt the
				action again - typically the NEXT render cycle (so in the mean time the lock
				can be cleared), or you can delegate that call to another thread in your
				app. You should not spin or put-to-sleep the render thread.

	@constant	kAUGraphErr_InvalidAudioUnit
*/
CF_ENUM(OSStatus)
{
	kAUGraphErr_NodeNotFound 				= -10860,
	kAUGraphErr_InvalidConnection 			= -10861,
	kAUGraphErr_OutputNodeErr				= -10862,
	kAUGraphErr_CannotDoInCurrentContext	= -10863,
	kAUGraphErr_InvalidAudioUnit			= -10864
};

#pragma mark -
/*!
    @function	NewAUGraph
    @abstract   creates a new AUGraph
    @discussion	creates a new AUGraph
    @param		outGraph		the new AUGraph object
*/
extern OSStatus
NewAUGraph(			AUGraph	__nullable * __nonnull outGraph)		__OSX_AVAILABLE_STARTING(__MAC_10_0,__IPHONE_2_0);

/*!
    @function	DisposeAUGraph
    @abstract   dispose an AUGraph
    @discussion	deallocates the AUGraph along with its nodes and their resources.
    @param		inGraph		the AUGraph object to be disposed
*/
extern OSStatus
DisposeAUGraph(		AUGraph			inGraph)						__OSX_AVAILABLE_STARTING(__MAC_10_0,__IPHONE_2_0);


#pragma mark -
#pragma mark Node State
/*! 
	@functiongroup AU Node management
*/

/*!
    @function	AUGraphAddNode
    @abstract   Add a node to an AUGraph
    @discussion	This creates a node in the graph that is an AudioUnit, using the supplied
				AudioComponentDescription to find and open that unit.
    @param		inGraph			the AUGraph object
    @param		inDescription	the AudioComponentDescription used to find and open the AudioUnit
    @param		outNode			the newly added node
*/
extern OSStatus
AUGraphAddNode(		AUGraph								inGraph,
					const AudioComponentDescription	*	inDescription,
					AUNode *	  						outNode)	__OSX_AVAILABLE_STARTING(__MAC_10_5,__IPHONE_2_0);

/*!
    @function	AUGraphRemoveNode
    @abstract   Remove a node from an AUGraph
    @discussion	Nodes can be removed from any thread context. The output node of
				the AUGraph cannot be removed while the graph is running.
    @param		inGraph			the AUGraph object
    @param		inNode			the node to be removed
*/
extern OSStatus
AUGraphRemoveNode(  AUGraph			inGraph,
					AUNode			inNode)							__OSX_AVAILABLE_STARTING(__MAC_10_0,__IPHONE_2_0);

/*!
    @function	AUGraphGetNodeCount
    @abstract   The number of nodes in an AUGraph
    @discussion	The number of nodes in an AUGraph
    @param		inGraph				the AUGraph object
    @param		outNumberOfNodes	the number of nodes
*/
extern OSStatus
AUGraphGetNodeCount(AUGraph			inGraph,
					UInt32 			*outNumberOfNodes)				__OSX_AVAILABLE_STARTING(__MAC_10_0,__IPHONE_2_0);

/*!
    @function	AUGraphGetIndNode
    @abstract   Returns the node at a given index
    @discussion	By using AUGraphGetNodeCount in conjunction with this call, you can
				iterate through the nodes of an AUGraph.
    @param		inGraph				the AUGraph object
    @param		inIndex				the index of the node to retrieve
	@param		outNode				the node at that index
*/
extern OSStatus
AUGraphGetIndNode(  AUGraph			inGraph,
					UInt32 			inIndex,
					AUNode			*outNode)						__OSX_AVAILABLE_STARTING(__MAC_10_0,__IPHONE_2_0);

/*!
    @function	AUGraphNodeInfo
    @abstract   Returns information about a particular AUNode
    @discussion	You can pass in NULL for any of the out parameters if you're not interested
				in that value.
    @param		inGraph				the AUGraph object
	@param		inNode				the node to query
    @param		outDescription		the component description that would describe the AudioUnit of this node
	@param		outAudioUnit		the AudioUnit of this node
*/
extern OSStatus
AUGraphNodeInfo(	AUGraph									inGraph,
					AUNode									inNode,
					AudioComponentDescription * __nullable	outDescription,
					AudioUnit __nullable * __nullable		outAudioUnit)		__OSX_AVAILABLE_STARTING(__MAC_10_5,__IPHONE_2_0);
			

#pragma mark -
#pragma mark Sub Graphs
/*! 
	@functiongroup Sub Graph Nodes
*/
/*!
    @function	AUGraphNewNodeSubGraph
    @abstract   Create a node that will represent a sub graph
	@discussion	This will create a node that represents a contained or member AUGraph.
				The AUGraph can be retrieved through the GetNodeInfoSubGraph call.
				The member AUGraph is owned by the parent graph and will be disposed when
				either:
				(1) The parent graph is disposed
				(2) The node is removed from the parent AUGraph
    @param		inGraph				the AUGraph object
	@param		outNode				the node that is used to refer to the sub-graph
*/
extern OSStatus
AUGraphNewNodeSubGraph( AUGraph				inGraph,
						AUNode				*outNode)			__OSX_AVAILABLE_STARTING(__MAC_10_2,__IPHONE_NA);

/*!
    @function	AUGraphGetNodeInfoSubGraph
    @abstract   Return an AUGraph represented by this node
	@discussion	This will return the sub graph represented by this AUNode.
    @param		inGraph				the AUGraph object
	@param		inNode				the node to query
	@param		outSubGraph			the sub-graph
*/
extern OSStatus
AUGraphGetNodeInfoSubGraph(	const AUGraph		inGraph,
							AUNode				inNode,
							AUGraph	__nullable * __nonnull outSubGraph)	__OSX_AVAILABLE_STARTING(__MAC_10_2,__IPHONE_NA);
								
/*!
    @function	AUGraphIsNodeSubGraph
    @abstract   Does the node represent a sub graph
	@discussion	This will return true if the specified node represents a subgraph, false if not.
    @param		inGraph				the AUGraph object
	@param		inNode				the node to query
	@param		outFlag				true if the node is a subgraph, false if not
*/
extern OSStatus
AUGraphIsNodeSubGraph(		const AUGraph		inGraph,
							AUNode				inNode,
							Boolean *			outFlag)		__OSX_AVAILABLE_STARTING(__MAC_10_2,__IPHONE_NA);


#pragma mark -
#pragma mark Node Interactions
/*! 
	@functiongroup Node Interactions
*/
/*! 
	@enum
	@abstract	The different types of node interactions
	@discussion We use the term interaction as a general term to describe some interaction 
				of a node. Currently, the interactions of a node that the graph manages are
				described below.
	@constant	kAUNodeInteraction_Connection
				connections between 2 audio units, 
	@constant	kAUNodeInteraction_InputCallback
				input callbacks being registered to a single audio unit's input bus.
*/
CF_ENUM(UInt32) {
	kAUNodeInteraction_Connection		= 1,
	kAUNodeInteraction_InputCallback	= 2
};

/*! 
	@struct		AudioUnitNodeConnection
	@abstract	A connection between two nodes
	@field		sourceNode
	@field		sourceOutputNumber
	@field		destNode
	@field		destInputNumber
*/
struct AudioUnitNodeConnection
{
	AUNode		sourceNode;
	UInt32		sourceOutputNumber;
	AUNode		destNode;
	UInt32		destInputNumber;
};
typedef struct AudioUnitNodeConnection AudioUnitNodeConnection;
typedef struct AudioUnitNodeConnection AUNodeConnection;

/*! 
	@struct		AUNodeRenderCallback
	@abstract	A callback used to provide input to an audio unit
	@discussion	Used to contain information when a callback is used
				to provide input to the specific node's specified input
	@field		destNode
	@field		destInputNumber
	@field		cback
*/
struct AUNodeRenderCallback 
{
	AUNode					destNode;
	AudioUnitElement		destInputNumber;
	AURenderCallbackStruct	cback;
};
typedef struct AUNodeRenderCallback AUNodeRenderCallback;

/*! 
	@struct		AUNodeInteraction
	@abstract	Used to describe the interaction between/with a node
	@discussion	This structure contains information about the interaction between
				two nodes (in the case of a connection), or the input to a node
				(in the case of a callback). 
				
				The type of the interaction is used to determine how to interpret the contents 
				of the following union.

				There may be other nodal interactions in the future, so NO ASSUMPTIONS should be made
				that these are the only 2 nodal interaction types; you must always check the 
				nodeInteractionType and only act on those types you understand
	
				Arrays of these structs can be returned, the addition of new members to the
				nodeInteraction union will NOT change the size of this structure
				
	@field		nodeInteractionType		the interaction type
	@field		nodeInteraction			a union providing information about the specified interaction
*/
struct AUNodeInteraction
{
	UInt32		nodeInteractionType;
	
	union
	{
		AUNodeConnection		connection;		
		AUNodeRenderCallback	inputCallback;

	}			nodeInteraction;
};
typedef struct AUNodeInteraction AUNodeInteraction;

/*! 
	@function	AUGraphConnectNodeInput
	@abstract	connect a node's output to a node's input
*/
extern OSStatus
AUGraphConnectNodeInput(	AUGraph			inGraph,
							AUNode			inSourceNode,
							UInt32			inSourceOutputNumber,
							AUNode			inDestNode,
							UInt32			inDestInputNumber)		__OSX_AVAILABLE_STARTING(__MAC_10_0,__IPHONE_2_0);

/*! 
	@function	AUGraphSetNodeInputCallback
	@abstract	Set a callback for the specified node's specified input.
	@param		inGraph
	@param		inDestNode
	@param		inDestInputNumber
	@param		inInputCallback		The callback that will provide input data to the node
*/
extern OSStatus
AUGraphSetNodeInputCallback (AUGraph						inGraph,
							AUNode							inDestNode,
							UInt32							inDestInputNumber,
							const AURenderCallbackStruct *	inInputCallback) 
																	__OSX_AVAILABLE_STARTING(__MAC_10_5,__IPHONE_2_0);

/*! 
	@function	AUGraphDisconnectNodeInput
	@abstract	disconnect a node's input
	@discussion	This can be used to disconnect either a connection or callback interaction
				to the specified node input
	@param		inGraph
	@param		inDestNode
	@param		inDestInputNumber
*/
extern OSStatus
AUGraphDisconnectNodeInput(	AUGraph			inGraph,
							AUNode			inDestNode,
							UInt32			inDestInputNumber)		__OSX_AVAILABLE_STARTING(__MAC_10_0,__IPHONE_2_0);

/*! 
	@function	AUGraphClearConnections
	@abstract	clear all of the interactions in a graph
	@discussion	This will clear all connections and callback interactions of the nodes of a graph
	@param		inGraph
*/
extern OSStatus
AUGraphClearConnections(	AUGraph			inGraph)				__OSX_AVAILABLE_STARTING(__MAC_10_0,__IPHONE_2_0);

/*! 
	@function	AUGraphGetNumberOfInteractions
	@abstract	Retrieve the number of interactions of a graph
	@discussion	The number of node interactions currently being managed by the graph
	@param		inGraph
	@param		outNumInteractions
*/
extern OSStatus
AUGraphGetNumberOfInteractions(	AUGraph				inGraph,
								UInt32 *			outNumInteractions)	__OSX_AVAILABLE_STARTING(__MAC_10_5,__IPHONE_2_0);

/*! 
	@function	AUGraphGetInteractionInfo
	@abstract	Retrieve information about a particular interaction in a graph
	@discussion	Returns information about a particular interaction. 
				inInteractionIndex is based on the outNumInteractions value and is only valid if no 
				edits to the graph's state have been made.
				
				An app can iterate through the interactions (as with the nodes) of a graph by retrieving
				the number of interactions, and then iterating an index from 0 < numInteractions
	@param		inGraph
	@param		inInteractionIndex
	@param		outInteraction		the interaction information at the specified index
*/
extern OSStatus
AUGraphGetInteractionInfo(	AUGraph					inGraph,
							UInt32					inInteractionIndex,
							AUNodeInteraction *		outInteraction)		__OSX_AVAILABLE_STARTING(__MAC_10_5,__IPHONE_2_0);

/*! 
	@function	AUGraphCountNodeInteractions
	@abstract	Retrieve the number of interactions of a graph's node
	@discussion	The number of node interactions currently being managed by the graph for the specified node
	@param		inGraph
	@param		inNode
	@param		outNumInteractions
*/
extern OSStatus
AUGraphCountNodeInteractions(	AUGraph				inGraph,
								AUNode				inNode,
								UInt32 *			outNumInteractions)	__OSX_AVAILABLE_STARTING(__MAC_10_5,__IPHONE_2_0);

/*! 
	@function	AUGraphGetNodeInteractions
	@abstract	Retrieve information about the interactions in a graph for a given node
	@discussion	Retrieve information about the interactions in a graph for a given node 
	@param		inGraph
	@param		inNode
	@param		ioNumInteractions	
					on input, specifies the number of interactions that can be returned
					on output, specifies the number of interactions returned.
	@param		outInteractions		the interactions the specified node is involved in
*/
extern OSStatus
AUGraphGetNodeInteractions(	AUGraph					inGraph,
							AUNode					inNode,
							UInt32 *				ioNumInteractions,
							AUNodeInteraction *		outInteractions)	__OSX_AVAILABLE_STARTING(__MAC_10_5,__IPHONE_2_0);



#pragma mark -
/*! 
	@functiongroup Updating an AUGraph
*/
/*! 
	@function	AUGraphUpdate
	@abstract	Updates the state of a running AUGraph
	
	@discussion
	Call this after performing a series of "edits" on the AUGraph with calls 
	such as AUGraphConnectNodeInput() to finalize those edits.
	The call will be synchronous if outIsUpdated is NULL, 
	meaning it will block until the changes are incorporated
	into the graph OR an error is returned from the update process
	if outIsUpdated is non-NULL, then AUGraphUpdate() will return 
	immediately and outIsUpdated will equal "true" if the changes 
	were already made (no more changes to make) or "false" if changes are still
	outstanding.

	Calling from the AUGraphRenderNotification callback:
	Connection and Disconnection events can be completely processed
	in the render notification callback of the AUGraph
	Nodes can also be removed (except for the output node or a sub-graph of the AUGraph)
	as well. 

	Getting kAUGraphErr_CannotDoInCurrentContext as a result code:
	If AUGraphUpdate returns this result, then it means it was
	unable to process the update, due to an inability to safely
	alter the state of the graph (because another thread was accessing
	a call that relies on the graph's state having integrity).
	This result code is only a transitory state, which will pass as soon
	as your other thread's call to AUGraph (that has the lock) completes.

	If an error is encountered in the process of an update (say an invalid connection
	is attempted, a disconnection between nodes that are not connected, etc) on a running graph, 	
	then the call will return that error code. It only process events whilst it receives
	no error results. Thus, if an error is encountered, other events will not be
	processed until AUGraphUpdate is called again. This is done, in cases where
	the state of the graph could become inconsistent if further events were processed, so
	this decision is left up to you. The same applies to the "cant do" error - you have
	to explicitly call AUGraphUpdate again to have the processing of the events occur.

	@param		inGraph
	@param		outIsUpdated	if specified returns true if all of the edits were applied to the graph
*/
extern OSStatus
AUGraphUpdate(		AUGraph					inGraph,
					Boolean	 * __nullable	outIsUpdated)			__OSX_AVAILABLE_STARTING(__MAC_10_0,__IPHONE_2_0);

#pragma mark -
#pragma mark State Management
/*! 
	@functiongroup AUGraph state management
*/
/*!
	@function	AUGraphOpen
	@abstract	Open a graph
	@discussion AudioUnits are open but not initialized (no resource allocation occurs here)
	@param		inGraph
*/
extern OSStatus
AUGraphOpen(			AUGraph		inGraph)						__OSX_AVAILABLE_STARTING(__MAC_10_0,__IPHONE_2_0);

/*!
	@function	AUGraphClose
	@abstract	Close a graph
	@discussion all AudioUnits are closed - leaving only its nodal representation
	@param		inGraph
*/
extern OSStatus
AUGraphClose(			AUGraph		inGraph)						__OSX_AVAILABLE_STARTING(__MAC_10_0,__IPHONE_2_0);

/*!
	@function	AUGraphInitialize
	@abstract	Initialise a graph
	@discussion AudioUnitInitialize() is called on each opened node/AudioUnit
				(get ready to render) and SubGraph that are involved in a
				interaction. If the node is not involved, it is initialised
				after it becomes involved in an interaction.
				
				A graph must be opened before it can be initialised.
	@param		inGraph
*/
extern OSStatus
AUGraphInitialize(		AUGraph		inGraph)						__OSX_AVAILABLE_STARTING(__MAC_10_0,__IPHONE_2_0);

/*!
	@function	AUGraphUninitialize
	@abstract	Uninitialise a graph
	@discussion The member of the graph are uninitialised 
	@param		inGraph
*/
extern OSStatus
AUGraphUninitialize(	AUGraph		inGraph)						__OSX_AVAILABLE_STARTING(__MAC_10_0,__IPHONE_2_0);

/*!
	@function	AUGraphStart
	@abstract	Start a graph
	@discussion Start() is called on the "head" node(s) of the AUGraph	(now rendering starts) 
				
				The graph must be initialised before it can be started
	@param		inGraph
*/
extern OSStatus
AUGraphStart(			AUGraph		inGraph)						__OSX_AVAILABLE_STARTING(__MAC_10_0,__IPHONE_2_0);

/*!
	@function	AUGraphStop
	@abstract	Stop a graph
	@discussion Stop() is called on the "head" node(s) of the AUGraph	(rendering is stopped)
*/
extern OSStatus
AUGraphStop(			AUGraph		inGraph)						__OSX_AVAILABLE_STARTING(__MAC_10_0,__IPHONE_2_0);


/*!
	@function	AUGraphIsOpen
	@abstract	Is the graph open
*/
extern OSStatus
AUGraphIsOpen(			AUGraph		inGraph,
						Boolean		*outIsOpen)						__OSX_AVAILABLE_STARTING(__MAC_10_0,__IPHONE_2_0);
							
/*!
	@function	AUGraphIsInitialized
	@abstract	Is the graph initialised
*/
extern OSStatus
AUGraphIsInitialized(	AUGraph		inGraph,
						Boolean		*outIsInitialized)				__OSX_AVAILABLE_STARTING(__MAC_10_0,__IPHONE_2_0);
							
/*!
	@function	AUGraphIsRunning
	@abstract	Is the graph running (has it been started)
*/
extern OSStatus
AUGraphIsRunning(		AUGraph		inGraph,
						Boolean		*outIsRunning)					__OSX_AVAILABLE_STARTING(__MAC_10_0,__IPHONE_2_0);
						
#pragma mark -
#pragma mark Utilities
/*! 
	@functiongroup AUGraph utilities
*/
/*!
	@function	AUGraphGetCPULoad
	@abstract	The CPU load of the graph
	@discussion	Returns a short-term running average of the current CPU load
				of the graph.		
*/
extern OSStatus
AUGraphGetCPULoad(		AUGraph		inGraph,
						Float32		*outAverageCPULoad)				__OSX_AVAILABLE_STARTING(__MAC_10_1,__IPHONE_2_0);

/*!
	@function	AUGraphGetMaxCPULoad
	@abstract	The Maximum CPU load of the graph
	@discussion	Returns the max CPU load of the graph since this call was last made 
				or the graph was last started.
*/
extern OSStatus
AUGraphGetMaxCPULoad(	AUGraph		inGraph,
						Float32		*outMaxLoad)					__OSX_AVAILABLE_STARTING(__MAC_10_3,__IPHONE_2_0);

/*!
	@function	AUGraphAddRenderNotify
	@abstract	Add a notification callback
	@discussion	Add a callback that the graph will call every time the
				graph renders. The callback will be called once before the graph's render
				operation, and once after the render operation is complete.
*/
extern OSStatus
AUGraphAddRenderNotify(			AUGraph					inGraph,
								AURenderCallback 		inCallback,
								void * __nullable		inRefCon)		__OSX_AVAILABLE_STARTING(__MAC_10_2,__IPHONE_2_0);

/*!
	@function	AUGraphRemoveRenderNotify
	@abstract	Remove a notification callback
	@discussion	Remove a previously added callback. You must provide both the callback
				and the refCon that was used previously to add the callback.
*/
extern OSStatus
AUGraphRemoveRenderNotify(		AUGraph					inGraph,
								AURenderCallback 		inCallback,
								void * __nullable		inRefCon)		__OSX_AVAILABLE_STARTING(__MAC_10_2,__IPHONE_2_0);
	
#pragma mark -
#pragma mark Deprecated

struct ComponentDescription;
/*!
	@function			AUGraphNewNode
	@discussion		ClassInfo data should not be used with new nodes
	@deprecated	in 10.5, see AUGraphAddNode
*/
extern OSStatus
AUGraphNewNode(	AUGraph								inGraph,
				const struct ComponentDescription	*inDescription,
				UInt32								inClassDataSize,	// reserved: must be zero
				const void							*inClassData,
				AUNode								*outNode)				__OSX_AVAILABLE_BUT_DEPRECATED(__MAC_10_0,__MAC_10_5, __IPHONE_NA, __IPHONE_NA);


/*!
	@function	AUGraphGetNodeInfo
	@discussion		ClassInfo data should not be used with new nodes
	@deprecated	in 10.5, see AUGraphNodeInfo
*/
extern OSStatus
AUGraphGetNodeInfo(	AUGraph						inGraph,
					AUNode						inNode,
					struct ComponentDescription *outDescription,
					UInt32						*outClassDataSize,
					void * __nullable * __nullable outClassData,
					AudioUnit __nullable * __nullable outAudioUnit)				__OSX_AVAILABLE_BUT_DEPRECATED(__MAC_10_0,__MAC_10_5, __IPHONE_NA, __IPHONE_NA);

/*!
	@function			AUGraphGetNumberOfConnections
	@deprecated	in 10.5, see AUGraphGetNumberOfInteractions
*/
extern OSStatus
AUGraphGetNumberOfConnections(	AUGraph		inGraph,
								UInt32		*outNumConnections)		__OSX_AVAILABLE_BUT_DEPRECATED(__MAC_10_1,__MAC_10_5, __IPHONE_NA, __IPHONE_NA);

/*!
	@function			AUGraphGetConnectionInfo
	@deprecated	in 10.5, see AUGraphGetInteractionInfo
*/
extern OSStatus
AUGraphGetConnectionInfo(	AUGraph		inGraph,
							UInt32		inConnectionIndex,
							AUNode		*outSourceNode,
							UInt32		*outSourceOutputNumber,
							AUNode		*outDestNode,
							UInt32		*outDestInputNumber)		__OSX_AVAILABLE_BUT_DEPRECATED(__MAC_10_1,__MAC_10_5, __IPHONE_NA, __IPHONE_NA);

/*!
	@function			AUGraphCountNodeConnections
	@deprecated	in 10.5, see AUGraphCountNodeInteractions
*/
extern OSStatus
AUGraphCountNodeConnections(	AUGraph 	inGraph,
								AUNode 		inNode,
								UInt32 		*outNumConnections)		__OSX_AVAILABLE_BUT_DEPRECATED(__MAC_10_3,__MAC_10_5, __IPHONE_NA, __IPHONE_NA);

/*!
	@function			AUGraphGetNodeConnections
	@deprecated	in 10.5, see AUGraphGetNodeInteractions
*/
extern OSStatus
AUGraphGetNodeConnections(		AUGraph						inGraph,
								AUNode						inNode,
								AudioUnitNodeConnection		*outConnections,
								UInt32						*ioNumConnections)	
																	__OSX_AVAILABLE_BUT_DEPRECATED(__MAC_10_3,__MAC_10_5, __IPHONE_NA, __IPHONE_NA);

	
#if defined(__cplusplus)
}
#endif

CF_ASSUME_NONNULL_END

#endif // AudioToolbox_AUGraph_h


