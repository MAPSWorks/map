/**
 * @file	Barrier.cpp 
 * @author	Jesús Carabaño Bravo <jcaraban@abo.fi>
 */

#include "Barrier.hpp"
#include "../visitor/Visitor.hpp"
#include <functional>


namespace map { namespace detail {

// Internal declarations

Barrier::Key::Key(Barrier *node) {
	prev = node->prev();
}

bool Barrier::Key::operator==(const Key& k) const {
	return (prev==k.prev);
}

std::size_t Barrier::Hash::operator()(const Key& k) const {
	return std::hash<Node*>()(k.prev);
}

// Factory

Node* Barrier::Factory(Node *arg) {
	assert(arg != nullptr);

	DataSize ds = arg->datasize();
	DataType dt = arg->datatype();
	MemOrder mo = arg->memorder();
	BlockSize bs = arg->blocksize();
	MetaData meta(ds,dt,mo,bs);

	return new Barrier(meta,arg);
}

// Constructors & methods

Barrier::Barrier(const MetaData &meta, Node *prev) : Node(meta) {
	prev_list.resize(1);
	prev_list[0] = prev;

	this->prev()->addNext(this);
}

void Barrier::accept(Visitor *visitor) {
	visitor->visit(this);
}

std::string Barrier::getName() const {
	return "Barrier";
}

std::string Barrier::signature() const {
	std::string sign = "";
	sign += classSignature();
	sign += prev()->numdim().toString();
	sign += prev()->datatype().toString();
	return sign;
}
/*
Node*& Barrier::prev() {
	return prev_list[0];
}
*/
Node* Barrier::prev() const {
	return prev_list[0];
}

} } // namespace map::detail
