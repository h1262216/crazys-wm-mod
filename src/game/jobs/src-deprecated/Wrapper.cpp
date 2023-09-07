#include "deprecated/Wrapper.h"

void JobWrapper::DoWork(cGirlShift& shift) const {
    auto result = m_Wrapped->Work(shift.girl(), shift.is_night_shift(), shift.rng());
    shift.data().Wages = result.Wages;
    shift.data().Earnings = result.Earnings;
    shift.data().Tips = result.Tips;
    //result.Refused = data.Refused != ECheckWorkResult::ACCEPTS;
}

bool JobWrapper::CheckCanWork(cGirlShift& shift) const {
    return true;
}

bool JobWrapper::CheckRefuseWork(cGirlShift& shift) const {
    return false;
}

double JobWrapper::GetPerformance(const sGirl& girl, bool estimate) const {
    return m_Wrapped->GetPerformance(girl, estimate);
}
