from AlCapExceptions import *

## \ingroup jobscripts
#  \author John R Quirk
#
#  \brief
#  Stores information parsed from the SGE
#  qstat command about a job
class SGEJob:
    ## \name Statuses
    #
    #  \brief
    #  Known SGE statuses
    #@{
    _WAITING = "qw"
    _RUNNING = "r"
    _RESTARTED = "R"
    _TRANSFERING = "t"
    _THRESHOLDSUSPEND = "T"
    _REQUESTEDSUSPEND = "s"
    _NODESUSPEND = "S"
    _DELETING = "d"
    _ERROR = "E"
    _HOLDING = "h"
    #@}

    ## \name Programs
    #
    #  \brief
    #  Known programs to run on grid (with
    #  values corresponding to their representation
    #  in the qstat output).
    #@{
    _ALCAPANA = "alcapana"
    _ROOTANA = "rootana"
    #@}

    _STATUSES = [_WAITING, _RUNNING, _RESTARTED, _TRANSFERING,
                 _THRESHOLDSUSPEND, _REQUESTEDSUSPEND,
                 _NODESUSPEND, _DELETING, _ERROR, _HOLDING]
    _PROGRAMS = [_ALCAPANA, _ROOTANA]

    def __init__(self, job_id, status, prog):
        self.job_id = job_id
        if prog not in SGEJob._PROGRAMS:
            raise UnknownProductionError(prog)
        self.program = prog
        self.SetStatus(status)

    def __eq__(self, rhs):
        if isinstance(rhs, self.__class__):
            return self.job_id == rhs.job_id
        return False

    ## \brief
    #  Set the status of this job to one of the understood states
    #  a grid job can be in.
    #
    #  \todo Check that conflicting statuses are not mutually set.
    def SetStatus(self, state):
        self.status = []
        for status in SGEJob._STATUSES:
            if status in state:
                self.status.append(status)
                state = state.replace(status, "")
        if len(state) != 0:
            msg = "There is an unknown state: " + state
            raise GridError(msg)
        return

    ## \brief
    #  Check if there is an error in the job.
    def HasError(self):
        if SGEJob._ERROR in self.status:
            return True
        return False
