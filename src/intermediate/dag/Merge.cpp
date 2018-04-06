/**
 * @file	Merge.cpp
 * @author	Jesús Carabaño Bravo <jcaraban@abo.fi>
 *
 * TODO: rethinking the merging of nodes with different dimension and precision
 */

#include "Merge.hpp"
#include "Constant.hpp"
#include "../../visitor/Visitor.hpp"
#include <functional>


namespace map { namespace detail {

// Internal declarations

Merge::Content::Content(Merge *node) {
	lprev = node->left();
	rprev = node->right();
	pat = node->pattern();
}

bool Merge::Content::operator==(const Content& k) const {
	return (lprev==k.lprev && rprev==k.rprev);
}

std::size_t Merge::Hash::operator()(const Content& k) const {
	return std::hash<Node*>()(k.lprev) ^ std::hash<Node*>()(k.rprev);
}

// Factory

Node* Merge::Factory(Node *lhs, Node *rhs, MergeLoopFlag flag) {
	assert(lhs != nullptr);
	assert(rhs != nullptr);

	MetaData meta;
	MetaData lmt = lhs->metadata();
	MetaData rmt = rhs->metadata();

	//if (lmt.getNumDim() == D0) {
	//	meta = rmt;
	//} else if (rmt.getNumDim() == D0) {
	//	meta = lmt;
	//} else {
		//assert(lmt.stream_dir == rmt.stream_dir);
		assert(lmt.getDataType() == rmt.getDataType());
		assert(lmt.getNumDim() == rmt.getNumDim());
		assert(lmt.getMemOrder() == rmt.getMemOrder());
		assert(lmt.getDataShape() == rmt.getDataShape());
		meta = lmt;
		if (lmt.getStreamDir() == IN && rmt.getStreamDir() == IN)
			meta.stream_dir = IN;
		else
			meta.stream_dir = IO;
	//}
	//meta.data_type = promote(lmt.getDataType(),rmt.getDataType());

	return new Merge(meta,lhs,rhs,flag);
}

Node* Merge::clone(const std::unordered_map<Node*,Node*> &other_to_this) {
	return new Merge(this,other_to_this);
}

// Constructors

Merge::Merge(const MetaData &meta, Node *lprev, Node *rprev, MergeLoopFlag not_used)
	: Node(meta)
{
	// Merge of a structured while loop	
	prev_list.reserve(1);
	this->addPrev(lprev);
	forw_list.reserve(1);
	this->addForw(rprev);

	if (lprev->file == rprev->file && lprev->file)
		this->file = rprev->file;

	lprev->addNext(this);
	rprev->addBack(this);
	
	//this->spatial_pattern = lprev->pattern() + rprev->pattern();
	this->in_spatial_reach = Mask(numdim().unitVec(),true);
	this->out_spatial_reach = Mask(numdim().unitVec(),true);
}

Merge::Merge(const MetaData &meta, Node *lprev, Node *rprev, MergeIfFlag not_used)
	: Node(meta)
{
	// Merge of a structured if-else
	assert(0); // @ not this yet
		
	prev_list.reserve(2);
	this->addPrev(lprev);
	this->addPrev(rprev);
	
	lprev->addNext(this);
	rprev->addNext(this);

	//this->spatial_pattern = lprev->pattern() + rprev->pattern();
	this->in_spatial_reach = Mask(numdim().unitVec(),true);
	this->out_spatial_reach = Mask(numdim().unitVec(),true);
}

Merge::Merge(const Merge *other, const std::unordered_map<Node*,Node*> &other_to_this)
	: Node(other,other_to_this)
{ }

// Methods

void Merge::accept(Visitor *visitor) {
	visitor->visit(this);
}

std::string Merge::shortName() const {
	return "Merge";
}

std::string Merge::longName() const {
	auto lid = std::to_string(left()->id);
	auto rid = std::to_string(right()->id);
	std::string str = "Merge {" + lid + "," + rid + "}";
	return str;
}

std::string Merge::signature() const {
	std::string sign = "";
	sign += classSignature();
	sign += left()->numdim().toString();
	sign += left()->datatype().toString();
	sign += right()->numdim().toString();
	sign += right()->datatype().toString();
	return sign;
}

Node* Merge::left() const {
	return prev_list[0]; // first element
}

Node* Merge::right() const {
	assert(prev_list.size() + forw_list.size() == 2);
	return (prev_list.size() == 2) ? prev_list[1] : forw_list[0];
}

// Compute

void Merge::computeScalar(std::unordered_map<Node*,VariantType> &hash) {
	assert(numdim() == D0);
	auto *node = this;

	bool left_found = hash.find(left()) != hash.end();
	bool right_found = hash.find(right()) != hash.end();
	assert(left_found xor right_found);

	VariantType value;
	if (left_found)
		value = hash.find(left())->second;
	else // right_found
		value = hash.find(right())->second;

	hash[node] = value;
}

void Merge::computeFixed(Coord coord, std::unordered_map<Key,ValFix,key_hash> &hash) {
	ValFix vf = ValFix();

	bool left_found = hash.find({left(),coord}) != hash.end();
	bool right_found = hash.find({right(),coord}) != hash.end();
	assert(left_found xor right_found);

	if (left_found) {
		vf = hash.find({left(),coord})->second;
	} else { // right_found
		vf = hash.find({right(),coord})->second;
	}
	
	hash[{this,coord}] = vf;
}

} } // namespace map::detail