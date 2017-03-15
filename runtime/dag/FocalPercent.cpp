/**
 * @file	FocalPercent.cpp 
 * @author	Jesús Carabaño Bravo <jcaraban@abo.fi>
 */

#include "FocalPercent.hpp"
#include "../visitor/Visitor.hpp"
#include <functional>


namespace map { namespace detail {

// Internal declarations

FocalPercent::Key::Key(FocalPercent *node) {
	prev = node->prev();
	smask = node->smask;
	type = node->type;
}

bool FocalPercent::Key::operator==(const Key& k) const {
	return (prev==k.prev && smask==k.smask && type==k.type);
}

std::size_t FocalPercent::Hash::operator()(const Key& k) const {
	return std::hash<Node*>()(k.prev) ^ k.smask.hash() ^ std::hash<int>()(k.type.get());
}

// Factory

Node* FocalPercent::Factory(Node *prev, const Mask &mask, PercentType type) {
	assert(prev != nullptr);
	assert(prev->numdim() != D0);
	assert(type != NONE_PERCENT);
	assert(prev->numdim() == mask.numdim());

	DataSize ds = prev->datasize();
	DataType dt = prev->datatype();
	MemOrder mo = prev->memorder();
	BlockSize bs = prev->blocksize();
	MetaData meta(ds,dt,mo,bs);

	return new FocalPercent(meta,prev,mask,type);
}

Node* FocalPercent::clone(NodeList new_prev_list) {
	return new FocalPercent(this,new_prev_list);
}

// Constructors

FocalPercent::FocalPercent(const MetaData &meta, Node *prev, const Mask &mask, PercentType type)
	: Node(meta)
{
	prev_list.reserve(1);
	this->addPrev(prev);
	this->smask = mask;
	this->type = type;
	
	prev->addNext(this);
}

FocalPercent::FocalPercent(const FocalPercent *other, NodeList new_prev_list)
	: Node(other,new_prev_list)
{
	this->smask = other->smask;
	this->type = other->type;
}

// Methods

void FocalPercent::accept(Visitor *visitor) {
	visitor->visit(this);
}

std::string FocalPercent::getName() const {
	return "FocalPercent";
}

std::string FocalPercent::signature() const {
	std::string sign = "";
	sign += classSignature();
	sign += prev()->numdim().toString();
	sign += prev()->datatype().toString();
	sign += mask().signature();
	sign += type.toString();
	return sign;
}
/*
Node*& FocalPercent::prev() {
	return prev_list[0];
}
*/
Node* FocalPercent::prev() const {
	return prev_list[0];
}

Mask FocalPercent::mask() const {
	return smask;
}

BlockSize FocalPercent::halo() const {
	return smask.datasize() / 2;
}

} } // namespace detail, map
