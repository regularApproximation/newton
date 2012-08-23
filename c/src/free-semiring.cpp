#include <sstream>
#include "free-semiring.h"

FreeSemiring::FreeSemiring()
{
	// Do not use this constructor. It is needed for a std::unordered_map...
}

FreeSemiring::FreeSemiring(Var var)
{
	this->type = Element;
	this->elem = new Var(var);
}

FreeSemiring::FreeSemiring(const FreeSemiring& term)
{
	this->type = term.type;
	switch(this->type)
	{
	case Element:
		this->elem = term.elem;
		break;
	case Addition:
	case Multiplication:
		this->right_ptr = term.right_ptr;
	case Star:
		this->left_ptr = term.left_ptr;
		break;
	}
}

FreeSemiring::FreeSemiring(optype type, FreeSemiring left, FreeSemiring right)
{
	this->type = type;
	this->left_ptr = std::make_shared<FreeSemiring>(left);
	this->right_ptr = std::make_shared<FreeSemiring>(right);
}

FreeSemiring::FreeSemiring(optype type, FreeSemiring left)
{
	this->type = type;
	this->left_ptr = std::make_shared<FreeSemiring>(left);
}

FreeSemiring::~FreeSemiring()
{
	//if(this->type == Element)
	//	delete this->elem;
}

FreeSemiring FreeSemiring::operator +(const FreeSemiring& term) const
{
	if(*this == FreeSemiring::null())
		return term;
	else if(term == FreeSemiring::null())
		return *this;
	else
		return FreeSemiring(Addition, *this, term);
}

FreeSemiring FreeSemiring::operator *(const FreeSemiring& term) const
{
	if(*this == FreeSemiring::one())
		return term;
	else if(term == FreeSemiring::one())
		return *this;
	else if(*this == FreeSemiring::null())
		return *this;
	else if(term == FreeSemiring::null())
		return term;
	else
		return FreeSemiring(Multiplication, *this, term);
}

bool FreeSemiring::operator ==(const FreeSemiring& term) const
{
	switch(this->type)
	{
		case Element:
			if(term.type != Element)
				return false;
			else
				return (*this->elem) == (*term.elem);
		case Addition:
		case Multiplication:
			return (this->type == term.type && this->left_ptr == term.left_ptr && this->right_ptr == term.right_ptr);
		case Star:
			return (this->type == term.type && this->left_ptr == term.left_ptr);
	}
	return false;
}

FreeSemiring FreeSemiring::star() const
{
	if(*this == FreeSemiring::null())
		return FreeSemiring::one();
	return FreeSemiring(Star, *this);
}

std::string FreeSemiring::string() const
{
	std::stringstream ss;

	switch(this->type)
	{
	case Element:
		ss << this->elem->string();
		break;
	case Addition:
		ss << "(";
		ss << this->left_ptr->string() << " + " << this->right_ptr->string();
		ss << ")";
		break;
	case Multiplication:
		ss << "(";
		ss << this->left_ptr->string() << " . " << this->right_ptr->string();
		ss << ")";
		break;
	case Star:
		//ss << "s(" << this->left_ptr->string() << ")";
		ss << "(";
		ss << this->left_ptr->string() << "*";
		ss << ")";
		break;
	}

	return ss.str();
}

FreeSemiring FreeSemiring::null()
{
	if(!FreeSemiring::elem_null)
		FreeSemiring::elem_null = new FreeSemiring(Var("Null"));
	return *FreeSemiring::elem_null;
}
FreeSemiring FreeSemiring::one()
{
	if(!FreeSemiring::elem_one)
		FreeSemiring::elem_one = new FreeSemiring(Var("One"));
	return *FreeSemiring::elem_one;
}

bool FreeSemiring::is_idempotent = false;
bool FreeSemiring::is_commutative = false;
FreeSemiring* FreeSemiring::elem_null = 0;
FreeSemiring* FreeSemiring::elem_one = 0;