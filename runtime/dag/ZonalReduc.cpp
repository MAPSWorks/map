/**
 * @file	ZonalReduc.cpp 
 * @author	Jesús Carabaño Bravo <jcaraban@abo.fi>
 */

#include "ZonalReduc.hpp"
#include "../visitor/Visitor.hpp"
#include <functional>


namespace map { namespace detail {

// Internal declarations

ZonalReduc::Key::Key(ZonalReduc *node) {
	prev = node->prev();
	type = node->type;
}

bool ZonalReduc::Key::operator==(const Key& k) const {
	return (prev==k.prev && type==k.type);
}

std::size_t ZonalReduc::Hash::operator()(const Key& k) const {
	return std::hash<Node*>()(k.prev) ^ std::hash<int>()(k.type.get());
}

// Factory

Node* ZonalReduc::Factory(Node *arg, ReductionType type) {
	assert(arg != nullptr);
	assert(arg->numdim() != D0);

	DataSize ds = DataSize(); // == D0
	DataType dt = arg->datatype();
	MemOrder mo = arg->memorder();
	BlockSize bs = BlockSize();
	MetaData meta(ds,dt,mo,bs);

	return new ZonalReduc(meta,arg,type);
}

// Constructors & methods

ZonalReduc::ZonalReduc(const MetaData &meta, Node *prev, ReductionType type) : Node(meta) {
	prev_list.resize(1);
	prev_list[0] = prev;
	this->type = type;
	this->value = type.neutral(datatype()); // @
	
	prev->addNext(this);
}

void ZonalReduc::accept(Visitor *visitor) {
	visitor->visit(this);
}

std::string ZonalReduc::getName() const {
	return "ZonalReduc";
}

std::string ZonalReduc::signature() const {
	std::string sign = "";
	sign += classSignature();
	sign += prev()->numdim().toString();
	sign += prev()->datatype().toString();
	sign += type.toString();
	return sign;
}
/*
Node*& ZonalReduc::prev() {
	return prev_list[0];
}
*/
Node* ZonalReduc::prev() const {
	return prev_list[0];
}

} } // namespace map::detail
