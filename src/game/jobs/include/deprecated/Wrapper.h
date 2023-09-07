#ifndef WM_JOB_WRAPPER_H
#define WM_JOB_WRAPPER_H

#include "jobs/cGenericJob.h"
#include "deprecated/IGenericJob.h"


class JobWrapper : public cGenericJob {
public:
    JobWrapper(std::unique_ptr<deprecated::IGenericJob> wrapped) :
        cGenericJob(wrapped->job()),
        m_Wrapped(std::move(wrapped))
    {
        m_Info.Name = m_Wrapped->get_info().Name;
        m_Info.Description = m_Wrapped->get_info().Description;
        m_Info.ShortName = m_Wrapped->get_info().ShortName;
        m_Info.JobId = m_Wrapped->get_info().JobId;
        m_Info.Shift = m_Wrapped->get_info().FullTime ? EJobShift::DAY : EJobShift::ANY;
        m_Info.FreeOnly = m_Wrapped->get_info().FreeOnly;
    }

    /// Gets an estimate or actual value of how well the girl performs at this job
    double GetPerformance(const sGirl& girl, bool estimate) const override;

    void DoWork(cGirlShift& shift) const override;

    bool CheckCanWork(cGirlShift& shift) const override;
    bool CheckRefuseWork(cGirlShift& shift) const override;

private:
    std::unique_ptr<deprecated::IGenericJob> m_Wrapped;
};

template<class T>
auto wrap(std::unique_ptr<T>&& v) {
    return std::make_unique<JobWrapper>(std::move(v));
}

#endif //WM_JOB_WRAPPER_H
