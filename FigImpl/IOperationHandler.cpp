#include "IOperationHandler.hpp"

IOperationHandler::IOperationHandler() :
	m_status(Fig::ExecutionStatus::EXECUTING),
	m_specific_code(Fig::FIG_SPECIFIC_CODE_RESERVED)
{
}

Fig::ExecutionStatus IOperationHandler::status()
{
	return m_status;
}

Fig::FigSpecificCode IOperationHandler::specific_code()
{
	return m_specific_code;
}

void IOperationHandler::error(const Fig::FigSpecificCode specific_code)
{
	m_status = Fig::ExecutionStatus::FAILED;
	m_specific_code = specific_code;
}
