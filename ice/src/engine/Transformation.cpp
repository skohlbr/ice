/*
 * Transformation.cpp
 *
 *  Created on: Nov 20, 2015
 *      Author: sni
 */

#include "ice/representation/Transformation.h"

#include "ice/representation/GContainer.h"
#include "ice/representation/GContainerFactory.h"

#include <muParser.h>

namespace ice
{

Transformation::Transformation(std::shared_ptr<ice::GContainerFactory> factory, std::string name, std::string scope,
                               std::shared_ptr<Representation> targetRepresentation) :
    factory(factory), name(name), scope(scope), targetRepresentation(targetRepresentation)
{
}

Transformation::~Transformation()
{
  for (auto op : this->operations)
  {
    delete op;
  }
}

std::shared_ptr<GContainer> Transformation::transform(std::shared_ptr<GContainer>* inputs)
{
  auto target = this->factory->makeInstance(this->targetRepresentation);

  for (auto operation : this->operations)
  {
    switch (operation->type)
    {
      case (DEFAULT):
        target->set(operation->targetDimension, operation->value);
        break;
      case (USE):
        target->set(operation->targetDimension, inputs[operation->sourceIndex]->get(operation->sourceDimension));
        break;
      case (FORMULA):
      {
	int nvars = operation->varmap.size();

	if (nvars <= 0) {
	  /* no mapping set -> use single variable mode by adding it to map */
          operation->varmap.insert(std::make_pair(operation->varname, 
		std::make_pair(operation->sourceDimension, operation->sourceIndex)));
	  nvars++;
	}

        mu::Parser parser;
        parser.SetExpr(operation->formula);

	void *in_raw;
        double in[nvars];
	int i = 0;
	for(auto const &var: operation->varmap) {
	        in_raw = inputs[var.second.second]->get(var.second.first);
		in[i] = convertDouble(in_raw, operation->valueType);
		parser.DefineVar(var.first, &in[i]);
		i++;
	}

        const double out = parser.Eval();
        target->set(operation->targetDimension, 
		convertVoid(out, operation->valueType));
      }
        break;
      default:
        //TODO
        break;
    }
  }

  return target;
}

const std::string Transformation::getName() const
{
  return this->name;
}

std::shared_ptr<Representation> Transformation::getTargetRepresentation()
{
  return this->targetRepresentation;
}

std::vector<TransformationOperation*>& Transformation::getOperations()
{
  return this->operations;
}

std::vector<std::shared_ptr<Representation>>& Transformation::getInputs()
{
  return this->inputs;
}

void Transformation::print()
{
  std::cout << "Transformation: " << this->name << " for scope " << this->scope << std::endl;
  std::cout << "Inputs: " << this->inputs.size() << std::endl;

  for (auto input : this->inputs)
  {
    std::cout << "   " << input->name << std::endl;
  }

  std::cout << "Operations: " << this->operations.size() << std::endl;

  for (auto op : this->operations)
  {
    switch (op->type)
    {
      case DEFAULT:
        std::cout << "   " << this->targetRepresentation->pathToString(op->targetDimension) << " Default Value "
            << op->value << std::endl;
        break;
      case USE:
        std::cout << "   " << this->targetRepresentation->pathToString(op->targetDimension) << " Use from index "
            << op->sourceIndex << ": " << this->inputs.at(op->sourceIndex)->pathToString(op->sourceDimension)
            << std::endl;
        break;
      case FORMULA:
        std::cout << "   " << this->targetRepresentation->pathToString(op->targetDimension) << " Formula " << std::endl;
        break;
    }
  }
}

std::unique_ptr<std::vector<std::string>> Transformation::getASPRepreentation(std::string system)
{
  // example
//  iro(system1,allo2ego,any,any).
//  input2(system1,allo2ego,position,coords,none,1,1) :- iro(system1,allo2ego,any,any).
//  input(system1,allo2ego,position,coords,none,1,1) :- iro(system1,allo2ego,any,any).
//  output(system1,allo2ego,position,egoCoords,any).
//  metadataOutput(delay,system1,allo2ego,max,0,0).
//  metadataOutput(accuracy,system1,allo2ego,avg,0,1).
//  iroCost(system1,allo2ego,1).

  auto vec = std::unique_ptr<std::vector<std::string>>(new std::vector<std::string>());
  std::string iro = "iro(" + system + "," + this->name + ",any,none).";

  vec->push_back(iro);

  std::string output = "output(" + system + "," + this->name + "," + this->scope + "," + this->targetRepresentation->name + ",none).";
  vec->push_back(output);

  for (auto input : this->inputs)
  {
    std::string inStr = "input(" + system + "," + this->name + "," + this->scope + "," + input->name + ",none,1,1) :- " + iro;
    vec->push_back(inStr);
  }

  // TODO metadata?

  return std::move(vec);
}

double Transformation::convertDouble(void *data, BasicRepresentationType type)
{
  switch (type)
  {
    case BYTE:
      return (double)(*((char*)data));
      break;
    case UNSIGNED_BYTE:
      return (double)(*((unsigned char*)data));
      break;
    case INT:
      return (double)(*((int*)data));
      break;
    case SHORT:
      return (double)(*((short*)data));
      break;
    case LONG:
      return (double)(*((long*)data));
      break;
    case UNSIGNED_INT:
      return (double)(*((unsigned int*)data));
      break;
    case UNSIGNED_LONG:
      return (double)(*((unsigned long*)data));
      break;
    case FLOAT:
      return (double)(*((float*)data));
      break;
    case DOUBLE:
      return *((double*)data);
      break;
    default:
      std::cerr << "Error: Unsupported formula datatype!" << std::endl;
      break;
  }
}

void *Transformation::convertVoid(double val, BasicRepresentationType type) {
	void *out_raw;
        switch (type)
        {
          case BYTE:
            out_raw = (new char);
            *((char*)out_raw) = (char)val;
            break;
          case UNSIGNED_BYTE:
            out_raw = (new unsigned char);
            *((unsigned char*)out_raw) = (unsigned char)val;
            break;
          case INT:
            out_raw = (new int);
            *((int*)out_raw) = (int)val;
            break;
          case SHORT:
            out_raw = (new short);
            *((short*)out_raw) = (short)val;
            break;
          case LONG:
            out_raw = (new long);
            *((long*)out_raw) = (long)val;
            break;
          case UNSIGNED_INT:
            out_raw = (new unsigned int);
            *((unsigned int*)out_raw) = (unsigned int)val;
            break;
          case UNSIGNED_LONG:
            out_raw = (new unsigned long);
            *((unsigned long*)out_raw) = (unsigned long)val;
            break;
          case FLOAT:
            out_raw = (new float);
            *((float*)out_raw) = (float)val;
            break;
          case DOUBLE:
            out_raw = (new double);
            *((double*)out_raw) = (double)val;
            break;
          default:
            std::cerr << "Error: Unsupported formula datatype!" << std::endl;
            break;
        }
	return out_raw;
}

} /* namespace ice */
