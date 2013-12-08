from cmath import exp

from cmath import tanh

class test:
	def value(self,index,in0,in1,in2):
		self.input0 = (in0 - 0.459987)/0.0509152
		self.input1 = (in1 - 0.188581)/0.0656804
		self.input2 = (in2 - 134.719)/16.5033
		if index==0: return ((self.neuron0x9d88238()*1)+0);
		return 0.
	def neuron0x9d86750(self):
		return self.input0
	def neuron0x9d868e0(self):
		return self.input1
	def neuron0x9d86ab8(self):
		return self.input2
	def neuron0x9d86da8(self):
		input = -1.05391
		input = input + self.synapse0x9d86f80()
		input = input + self.synapse0x9d86fa8()
		input = input + self.synapse0x9d86fd0()
		return ((1/(1+exp(-input)))*1)+0
	def neuron0x9d86ff8(self):
		input = 0.527983
		input = input + self.synapse0x9d871f0()
		input = input + self.synapse0x9d87218()
		input = input + self.synapse0x9d87240()
		return ((1/(1+exp(-input)))*1)+0
	def neuron0x9d87268(self):
		input = -0.44426
		input = input + self.synapse0x9d87460()
		input = input + self.synapse0x9d87488()
		input = input + self.synapse0x9d874b0()
		return ((1/(1+exp(-input)))*1)+0
	def neuron0x9d874d8(self):
		input = 0.222012
		input = input + self.synapse0x9d876d0()
		input = input + self.synapse0x9d876f8()
		input = input + self.synapse0x9d87720()
		return ((1/(1+exp(-input)))*1)+0
	def neuron0x9d87748(self):
		input = -1.25597
		input = input + self.synapse0x9d87940()
		input = input + self.synapse0x9d87968()
		input = input + self.synapse0x9d87990()
		return ((1/(1+exp(-input)))*1)+0
	def neuron0x9d879d8(self):
		input = -2.38813
		input = input + self.synapse0x9d87bb0()
		input = input + self.synapse0x9d87bd8()
		input = input + self.synapse0x9d87c88()
		input = input + self.synapse0x9d87cb0()
		input = input + self.synapse0x9d87cd8()
		return ((1/(1+exp(-input)))*1)+0
	def neuron0x9d87d00(self):
		input = -0.0244718
		input = input + self.synapse0x9d87eb0()
		input = input + self.synapse0x9d87ed8()
		input = input + self.synapse0x9d87f00()
		input = input + self.synapse0x9d87f28()
		input = input + self.synapse0x9d87f50()
		return ((1/(1+exp(-input)))*1)+0
	def neuron0x9d87f78(self):
		input = 0.649539
		input = input + self.synapse0x9d88170()
		input = input + self.synapse0x9d88198()
		input = input + self.synapse0x9d881c0()
		input = input + self.synapse0x9d881e8()
		input = input + self.synapse0x9d88210()
		return ((1/(1+exp(-input)))*1)+0
	def neuron0x9d88238(self):
		input = -1.9076
		input = input + self.synapse0x9d88410()
		input = input + self.synapse0x9d88438()
		input = input + self.synapse0x9d88460()
		return (input*1)+0
	def synapse0x9d86f80(self):
		return (self.neuron0x9d86750()*-0.754043)
	def synapse0x9d86fa8(self):
		return (self.neuron0x9d868e0()*-1.75155)
	def synapse0x9d86fd0(self):
		return (self.neuron0x9d86ab8()*5.13315)
	def synapse0x9d871f0(self):
		return (self.neuron0x9d86750()*0.899592)
	def synapse0x9d87218(self):
		return (self.neuron0x9d868e0()*1.90885)
	def synapse0x9d87240(self):
		return (self.neuron0x9d86ab8()*0.0458485)
	def synapse0x9d87460(self):
		return (self.neuron0x9d86750()*-1.71223)
	def synapse0x9d87488(self):
		return (self.neuron0x9d868e0()*-0.177615)
	def synapse0x9d874b0(self):
		return (self.neuron0x9d86ab8()*0.776569)
	def synapse0x9d876d0(self):
		return (self.neuron0x9d86750()*1.08995)
	def synapse0x9d876f8(self):
		return (self.neuron0x9d868e0()*-3.98195)
	def synapse0x9d87720(self):
		return (self.neuron0x9d86ab8()*0.586277)
	def synapse0x9d87940(self):
		return (self.neuron0x9d86750()*0.707285)
	def synapse0x9d87968(self):
		return (self.neuron0x9d868e0()*-0.248171)
	def synapse0x9d87990(self):
		return (self.neuron0x9d86ab8()*-0.206521)
	def synapse0x9d87bb0(self):
		return (self.neuron0x9d86da8()*2.15972)
	def synapse0x9d87bd8(self):
		return (self.neuron0x9d86ff8()*-1.43131)
	def synapse0x9d87c88(self):
		return (self.neuron0x9d87268()*-2.70734)
	def synapse0x9d87cb0(self):
		return (self.neuron0x9d874d8()*2.09934)
	def synapse0x9d87cd8(self):
		return (self.neuron0x9d87748()*-4.69941)
	def synapse0x9d87eb0(self):
		return (self.neuron0x9d86da8()*0.493043)
	def synapse0x9d87ed8(self):
		return (self.neuron0x9d86ff8()*-0.334755)
	def synapse0x9d87f00(self):
		return (self.neuron0x9d87268()*-0.543533)
	def synapse0x9d87f28(self):
		return (self.neuron0x9d874d8()*-0.908362)
	def synapse0x9d87f50(self):
		return (self.neuron0x9d87748()*0.0706197)
	def synapse0x9d88170(self):
		return (self.neuron0x9d86da8()*0.0714893)
	def synapse0x9d88198(self):
		return (self.neuron0x9d86ff8()*1.06403)
	def synapse0x9d881c0(self):
		return (self.neuron0x9d87268()*1.47725)
	def synapse0x9d881e8(self):
		return (self.neuron0x9d874d8()*-0.322393)
	def synapse0x9d88210(self):
		return (self.neuron0x9d87748()*0.65899)
	def synapse0x9d88410(self):
		return (self.neuron0x9d879d8()*3.80993)
	def synapse0x9d88438(self):
		return (self.neuron0x9d87d00()*-0.846735)
	def synapse0x9d88460(self):
		return (self.neuron0x9d87f78()*2.50977)
