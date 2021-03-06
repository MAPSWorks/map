/**
 * @file	FocalFlow.cpp 
 * @author	Jesús Carabaño Bravo <jcaraban@abo.fi>
 */

#include "FocalFlow.hpp"
#include "../visitor/Visitor.hpp"
#include <functional>


namespace map { namespace detail {

// Internal declarations

FocalFlow::Key::Key(FocalFlow *node) {
	prev = node->prev();
}

bool FocalFlow::Key::operator==(const Key& k) const {
	return (prev==k.prev);
}

std::size_t FocalFlow::Hash::operator()(const Key& k) const {
	return std::hash<Node*>()(k.prev);
}

// Factory

Node* FocalFlow::Factory(Node *arg) {
	assert(arg != nullptr);
	assert(arg->numdim() != D0);

	DataSize ds = arg->datasize();
	DataType dt = U8;
	MemOrder mo = arg->memorder();
	BlockSize bs = arg->blocksize();
	MetaData meta(ds,dt,mo,bs);

	return new FocalFlow(meta,arg);
}

// Constructors & methods

FocalFlow::FocalFlow(const MetaData &meta, Node *prev) : Node(meta) {
	prev_list.resize(1);
	prev_list[0] = prev;
	
	prev->addNext(this);
}

void FocalFlow::accept(Visitor *visitor) {
	visitor->visit(this);
}

std::string FocalFlow::getName() const {
	return "FocalFlow";
}

std::string FocalFlow::signature() const {
	std::string sign = "";
	sign += classSignature();
	sign += prev()->numdim().toString();
	sign += prev()->datatype().toString();
	return sign;
}
/*
Node*& FocalFlow::prev() {
	return prev_list[0];
}
*/
Node* FocalFlow::prev() const {
	return prev_list[0];
}

BlockSize FocalFlow::halo() const {
	return BlockSize{1,1};//,1,1};
}

} } // namespace map::detail
