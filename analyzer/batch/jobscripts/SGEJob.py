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
    _ALCAPANA = "batch_alca"
    _ROOTANA = "batch_root"
    #@}

    _STATUSES = [_WAITING, _RUNNING, _RESTARTED, _TRANSFERING,
                 _THRESHOLDSUSPENDED, _REQUESTEDSUSPENDED,
                 _NODESUSPENDED, _DELETING, _ERROR, _HOLDING]
    _PROGRAMS = [_ALCAPANA, _ROOTANA]

    def __init__(self, job_id, status, prog):
        self.job_id = job_id
        if prog not in _PROGRAMS:
            msg = prog + "not a valid program (expect batch_alca or batch_root)!"
            print msg
            raise GridError(msg)
        self.program = prog
        self.SetStatus(status)

    def __eq__(self, rhs):
        if isinstance(rhs, self.__class__):
            return self.job_id == rhs.job_id
        else:
            return False

    ## \brief
    #  Set the status of this job to one of the understood states
    #  a grid job can be in.
    #
    #  \todo Check that conflicting statuses are not mutually set.
    def SetStaus(self, state):
        self.status = []
        for status in _STATUSES:
            if status in state:
                self.status.append(status)
                state = state.replace(status, "")
        if len(state) != 0:
            msg = "There is an unknown state: " + state
            print msg
            raise GridError(msg)
