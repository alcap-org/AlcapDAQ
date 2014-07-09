## \brief
#  Exception to derive other ones from so that,
#  if an exception is thrown, we know the problem
#  is probably one we expected, and not a bug in
#  the program.
class AlCapError(Exception):
    def __init__(self, value):
        self.value = value
    def __str__(self):
        return "AlCap Error: " + repr(self.value) + "!"

class ArgumentError(AlCapError):
    pass
    
class GridError(AlCapError):
    pass

class DataBaseError(AlCapError):
    pass

class UnknownProductionError(AlCapError):
    def __init__(self, value):
        self.value = "Unknown production type! (" + repr(value) + ")"

class UnknownAlcapanaFiletypeError(AlCapError):
    def __init__(self, value):
        self.value = "Unknown alcapana filetype! (" + repr(value) + ")"

class UnknownRootanaFiletypeError(AlCapError):
    def __init__(self, value):
        self.value = "Unknown rootana filetype! (" + repr(value) + ")"
