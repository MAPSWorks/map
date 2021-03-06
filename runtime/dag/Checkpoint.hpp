/**
 * @file	Checkpoint.hpp 
 * @author	Jesús Carabaño Bravo <jcaraban@abo.fi>
 *
 */

#ifndef MAP_RUNTIME_DAG_CHECKPOINT_HPP_
#define MAP_RUNTIME_DAG_CHECKPOINT_HPP_

#include "IO.hpp"


namespace map { namespace detail {

//typedef std::unique_ptr<IFile> AnyFile;

struct Checkpoint : public OutInNode
{
	// Internal declarations
	struct Key {
		Key(Checkpoint *node);
		bool operator==(const Key& k) const;
		Node *prev;
	};
	struct Hash {
		std::size_t operator()(const Key& k) const;
	};
	
	// Factory
	static Node* Factory(Node *prev);

	// Constructors & methods
	Checkpoint(Node *prev, SharedFile tmp_file);
	void accept(Visitor *visitor);
	std::string getName() const;
	std::string signature() const;
	char classSignature() const;

	// Variables
	//AnyFile file;
};

} } // namespace map::detail

#endif
