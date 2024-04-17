#define EQUATION_SET_SAVE_VERSION 18

enum EqNodeType {
  EQ_NODE_SOURCE=0,
  EQ_NODE_DEST=1,
  EQ_NODE_EASE=2,
  EQ_NODE_SINE=3,
  EQ_NODE_CLAMP=4, // Not used?
  EQ_NODE_MAP=5,
  EQ_NODE_MULTIPLY=6,
  EQ_NODE_ADD=7,
  EQ_NODE_FADE=8,
  EQ_NODE_SUBTRACT=9,
  EQ_NODE_DIVIDE=10,
  EQ_NODE_PERLIN=11,
  EQ_NODE_SPLIT=12,
  EQ_NODE_BATTERY=13,
  EQ_NODE_EQ=14,
  EQ_NODE_IF=15,
  EQ_NODE_MODULO=16,
  EQ_NODE_TICKER=17,
  EQ_NODE_ORDERED_RNG=18,
  EQ_NODE_SINK=19,
  EQ_NODE_TYPES_MAX,
};
char *eqNodeTypeStrings[] = {
  "Source",
  "Destination",
  "Ease",
  "Sine",
  "Clamp",
  "Map",
  "Multiply",
  "Add",
  "Fade",
  "Subtract",
  "Divide",
  "Perlin",
  "Split",
  "Battery",
  "Equation",
  "If",
  "Modulo",
  "Ticker",
  "Ordered Rng",
  "Sink",
};
struct EqNodeLink {
	s64 id;
	s64 inputPinId;
	s64 outputPinId;
};
char *eqTimeTypeStrings[] = {
	"Loop",
	"Once",
	"Infinite",
};
#define EQ_NODE_TICKER_ON_RISING_EDGE   (1 << 1)
#define EQ_NODE_TICKER_ON_FALLING_EDGE  (1 << 2)
struct EqNode {
	s64 id;

  EqNodeType type;
	Ease easeType;

	int timeType; // Unused

  float sineFreq;
  float sineAmp;
  float sineShift;

  float clampMin;
  float clampMax;

  Vec4 vec4Value;
  bool boolValue;

  float floatValue;
  int intValue;
#define EQ_NODE_STRING_VALUE_MAX_LEN 128
	char stringValue[EQ_NODE_STRING_VALUE_MAX_LEN];

#define EQ_NODE_PINS_MAX 8
	s64 inputPinIds[EQ_NODE_PINS_MAX];
	s64 outputPinIds[EQ_NODE_PINS_MAX];
  float constantInputs[EQ_NODE_PINS_MAX];

	Vec2 position;

	// unserialized
  int index;
  EqNodeLink *cachedInputLinks[EQ_NODE_PINS_MAX];
};
struct Equation {
#define EQUATION_NAME_MAX_LEN EQ_NODE_STRING_VALUE_MAX_LEN
  char name[EQUATION_NAME_MAX_LEN];
  char folder[EQUATION_NAME_MAX_LEN];

  EqNode *nodes;
  int nodesNum;

	EqNodeLink *links;
	int linksNum;

	s64 nextNodeId;

  char inputs[EQ_NODE_PINS_MAX][EQ_NODE_STRING_VALUE_MAX_LEN];
  int inputsNum;
  char outputs[EQ_NODE_PINS_MAX][EQ_NODE_STRING_VALUE_MAX_LEN];
  int outputsNum;
};
struct EquationSet {
  Equation *eqs;
  int eqsNum;
};
struct EqVar {
#define EQ_EVAL_VAR_NAME_MAX_LEN EQ_NODE_STRING_VALUE_MAX_LEN
	char name[EQ_EVAL_VAR_NAME_MAX_LEN];
	float value;
};
struct EqNodeEvalData {
	s64 relatedNodeId;
  bool evaled;
  float outputs[EQ_NODE_PINS_MAX];

  float batteryValue;
	int tickerValue;
	float prevTickerInput;
	int rngSeed;

#define EQ_SIM_VALUES_MAX 500
  float simValues[EQ_NODE_PINS_MAX][EQ_SIM_VALUES_MAX];
  int simValuesNum;
};
struct EqEvalData {
  char eqName[EQUATION_NAME_MAX_LEN];
  char name[EQ_EVAL_VAR_NAME_MAX_LEN];
  s64 relatedNodeId;

#define EQ_EVAL_DATA_VARS_MAX 32
	EqVar vars[EQ_EVAL_DATA_VARS_MAX];
	int varsNum;

  BucketArray *nodeDataArray;
	BucketArray *subEvalDataArray;
};

struct EquationSystem {
  EquationSet *eqSet;

  ImNode::EditorContext *editorContext;

  bool editorFloating;
  bool prevEditorFloating;
  float editorAlpha;
  float toolSplitPerc;
  int selectedEqIndex;
  bool simNodes;
  EqEvalData *attachedEvalData;
  float timeSinceLastNodeRefresh;

	Equation *copiedNodesEq;
	int *copiedNodes;
	int copiedNodesNum;

#define EQUATION_EXPORTS_MAX 2048
	bool foldersToExport[EQUATION_EXPORTS_MAX];
	char exportPath[PATH_MAX_LEN];
	char importPath[PATH_MAX_LEN];

#define STORED_EVAL_DATAS_MAX 1024
  EqEvalData storedEvalDatas[STORED_EVAL_DATAS_MAX];
  int storedEvalDatasNum;

	int gridColor;
	int bgColor;
	int windowBgColor;
};

EquationSystem *eqSys;

void checkEquationSystemInit();
Equation *createEquation();
EqNode *createNode(Equation *eq, EqNodeType type);
EqNodeLink *createLink(Equation *eq, int inputPinId, int outputPinId);
float *step(EqEvalData *evalData, float dt);
float *step(Equation *eq, float dt, EqEvalData *evalData);
void eval(Equation *eq, EqNode *node, float subEqDt, EqEvalData *evalData);

Equation *getEquation(char *name);
EqNode *getNode(Equation *eq, s64 nodeId);
EqEvalData *getEvalData(char *eqName, char *name);

s64 getNodeIdThatHasInputPinId(Equation *eq, s64 inputPinId);
s64 getNodeIdThatHasOutputPinId(Equation *eq, s64 outputPinId);
s64 getNodeIdThatHasPinId(Equation *eq, s64 pinId);
s64 isInputPin(Equation *eq, s64 pinId);
s64 isOutputPin(Equation *eq, s64 pinId);

EqNode *getNodeThatHasThisPinId(Equation *eq, s64 pinId, int *pinIndex=NULL);
EqNodeLink *getLinkOutputtingToThisInputPinId(Equation *eq, int pinId);
EqNodeLink *getLinkConnectedToThisPinId(Equation *eq, int pinId);

void destroy(EqEvalData *evalData);
EqEvalData *reset(EqEvalData *evalData);

float getVar(EqEvalData *evalData, char *varName);
void setVar(EqEvalData *evalData, char *varName, float value);
float *getOutputs(EqEvalData *evalData);
Matrix3 getMatrixOutput(EqEvalData *evalData, Vec2 textureSize=v2());

EqNode *getNodeOfType(Equation *eq, EqNodeType nodeType, int *outIndex=NULL);
EqNodeEvalData *getNodeEvalData(EqNode *node, EqEvalData *evalData);
char **getFolderList(int *outFolderListNum); // Only exists for one frame
void copyEquation(Equation *dest, Equation *src);

void showNodeStyleEditor();
void guiShowEquationNodeWindow();
void updateEquationSystem(float elapsed);

void saveLoadEquation(DataStream *stream, bool save, int version, Equation *eq);
bool saveLoadEquationSet(DataStream *stream, bool save, int outerVersion, EquationSet *eqSet, int startVersion, int endVersion);
/// FUNCTIONS ^

void checkEquationSystemInit() {
  if (eqSys) return;
  eqSys = (EquationSystem *)zalloc(sizeof(EquationSystem));
}

Equation *createEquation() {
	if (eqSys->copiedNodesNum > 0) {
		logf("Clipboard of %d nodes has been cleared\n", eqSys->copiedNodesNum);
		eqSys->copiedNodesNum = 0;
	}

  EquationSet *eqSet = eqSys->eqSet;

  eqSet->eqs = (Equation *)resizeArray(eqSet->eqs, sizeof(Equation), eqSet->eqsNum, eqSet->eqsNum+1);
  Equation *eq = &eqSet->eqs[eqSet->eqsNum++];
  memset(eq, 0, sizeof(Equation));
  eq->inputsNum = 1;
	strcpy(eq->inputs[0], "t");
  snprintf(eq->name, EQUATION_NAME_MAX_LEN, "Equation %d", eqSet->eqsNum);
  return eq;
}

EqNode *createNode(Equation *eq, EqNodeType type) {
  if (type == EQ_NODE_DEST) {
    EqNode *existingDest = getNodeOfType(eq, EQ_NODE_DEST);
    if (existingDest) {
      logf("You can only have one destination node\n");
      return NULL;
    }
  }

	eq->nodes = (EqNode *)resizeArray(eq->nodes, sizeof(EqNode), eq->nodesNum, eq->nodesNum+1);
	EqNode *node = &eq->nodes[eq->nodesNum++];
	memset(node, 0, sizeof(EqNode));
	node->id = ++eq->nextNodeId;
	node->type = type;
  node->sineFreq = 1;
  node->sineAmp = 1;
	for (int i = 0; i < EQ_NODE_PINS_MAX; i++) node->inputPinIds[i] = ++eq->nextNodeId;
	for (int i = 0; i < EQ_NODE_PINS_MAX; i++) node->outputPinIds[i] = ++eq->nextNodeId;

	if (node->type == EQ_NODE_MAP) {
		node->vec4Value = v4(0, 1, 0, 1);
		node->boolValue = true;
	} else if (node->type == EQ_NODE_FADE) {
		node->vec4Value = v4(0, 1, 1, 0);
	} else if (node->type == EQ_NODE_PERLIN) {
		node->floatValue = 3;
		node->intValue = 2;
	} else if (node->type == EQ_NODE_SPLIT) {
		node->intValue = 1;
	}

	eqSys->timeSinceLastNodeRefresh = 0;
	auto s_Editor = reinterpret_cast<ImNode::Detail::EditorContext*>(eqSys->editorContext);
	auto imRect = s_Editor->GetViewRect();
	node->position.x = imRect.GetCenter().x;
	node->position.y = imRect.GetCenter().y;

	return node;
}

EqNodeLink *createLink(Equation *eq, int inputPinId, int outputPinId) {
	eq->links = (EqNodeLink *)resizeArray(eq->links, sizeof(EqNodeLink), eq->linksNum, eq->linksNum+1);
	EqNodeLink *link = &eq->links[eq->linksNum++];
	memset(link, 0, sizeof(EqNodeLink));
	link->id = eq->nextNodeId++;
	link->inputPinId = inputPinId;
	link->outputPinId = outputPinId;
	return link;
}

float *step(EqEvalData *evalData, float dt) {
  if (!evalData) return (float *)frameMalloc(sizeof(float) * EQ_NODE_PINS_MAX);

  Equation *eq = getEquation(evalData->eqName);
  if (!eq) return (float *)frameMalloc(sizeof(float) * EQ_NODE_PINS_MAX);

  return step(eq, dt, evalData);
}

float *step(Equation *eq, float dt, EqEvalData *evalData) {
	setVar(evalData, "t", getVar(evalData, "t") + dt);

	for (int i = 0; i < eq->nodesNum; i++) {
		EqNode *node = &eq->nodes[i];
    node->index = i;
		EqNodeEvalData *nodeEvalData = getNodeEvalData(node, evalData);
    nodeEvalData->evaled = false;
    for (int i = 0; i < EQ_NODE_PINS_MAX; i++) {
      node->cachedInputLinks[i] = getLinkOutputtingToThisInputPinId(eq, node->inputPinIds[i]);
    }
  }

	EqNode *destNode = getNodeOfType(eq, EQ_NODE_DEST);
	if (destNode) eval(eq, destNode, dt, evalData);

  for (int i = 0; i < eq->nodesNum; i++) {
    EqNode *node = &eq->nodes[i];
		if (node->type == EQ_NODE_SINK) eval(eq, node, dt, evalData);
	}

  return getOutputs(evalData);
}

void eval(Equation *eq, EqNode *node, float subEqDt, EqEvalData *evalData) {
  float inputs[EQ_NODE_PINS_MAX] = {};
  for (int i = 0; i < EQ_NODE_PINS_MAX; i++) {
    EqNodeLink *link = node->cachedInputLinks[i];
    if (link) {
			int outputPinIndexFromLinked = 0;
			EqNode *inputNode = getNodeThatHasThisPinId(eq, link->outputPinId, &outputPinIndexFromLinked);
			if (!inputNode) continue;

      EqNodeEvalData *inputNodeEvalData = getNodeEvalData(inputNode, evalData);
      if (!inputNodeEvalData->evaled) eval(eq, inputNode, subEqDt, evalData);

			inputs[i] = inputNodeEvalData->outputs[outputPinIndexFromLinked];
		} else {
			inputs[i] = node->constantInputs[i];
		}
  }

	bool isTimeBased = false;
	if (node->type == EQ_NODE_SOURCE) {
		isTimeBased = true;
	}

  EqNodeEvalData *nodeEvalData = getNodeEvalData(node, evalData);
  float *outputs = nodeEvalData->outputs;

  if (node->type == EQ_NODE_SOURCE) {
		char *varName = node->stringValue;
		if (!varName[0]) varName = "t";
		float t = getVar(evalData, varName);
		outputs[0] = t;
  } else if (node->type == EQ_NODE_DEST) {
    for (int i = 0; i < EQ_NODE_PINS_MAX; i++) outputs[i] = inputs[i];
  } else if (node->type == EQ_NODE_EASE) {
		outputs[0] = tweenEase(inputs[0], node->easeType);
  } else if (node->type == EQ_NODE_SINE) {
    float sinInput = inputs[0] * 2 * M_PI;
    float value = sin((sinInput + node->sineShift) * node->sineFreq);
    value *= node->sineAmp;
    value = (value+1) / 2;
    outputs[0] = value;
  } else if (node->type == EQ_NODE_CLAMP) {
    float value = inputs[0];
    if (value < node->clampMin) value = node->clampMin;
    if (value > node->clampMax) value = node->clampMax;
    outputs[0] = value;
  } else if (node->type == EQ_NODE_MAP) {
    if (node->boolValue) {
      outputs[0] = clampMap(inputs[0], node->vec4Value.x, node->vec4Value.y, node->vec4Value.z, node->vec4Value.w, node->easeType);
    } else {
      outputs[0] = map(inputs[0], node->vec4Value.x, node->vec4Value.y, node->vec4Value.z, node->vec4Value.w, node->easeType);
    }
  } else if (node->type == EQ_NODE_MULTIPLY) {
    outputs[0] = inputs[0] * inputs[1];
  } else if (node->type == EQ_NODE_ADD) {
    outputs[0] = inputs[0] + inputs[1];
  } else if (node->type == EQ_NODE_FADE) {
    float power = clampMap(inputs[1], node->vec4Value.x, node->vec4Value.y, node->vec4Value.z, node->vec4Value.w);
    outputs[0] = inputs[0] * power;
  } else if (node->type == EQ_NODE_SUBTRACT) {
    outputs[0] = inputs[0] - inputs[1];
  } else if (node->type == EQ_NODE_DIVIDE) {
    outputs[0] = inputs[0] / inputs[1];
  } else if (node->type == EQ_NODE_PERLIN) {
		if (node->intValue < 1) node->intValue = 1;
		outputs[0] = perlin2d(inputs[0], inputs[1], node->floatValue, node->intValue);
  } else if (node->type == EQ_NODE_SPLIT) {
    for (int i = 0; i < EQ_NODE_PINS_MAX; i++) outputs[i] = inputs[0];
  } else if (node->type == EQ_NODE_BATTERY) {
    float change = inputs[0];
    if (change) {
      nodeEvalData->batteryValue += change;
    } else {
      nodeEvalData->batteryValue -= node->vec4Value.y;
    }
    nodeEvalData->batteryValue = mathClamp(nodeEvalData->batteryValue, 0, node->vec4Value.x);
		outputs[0] = nodeEvalData->batteryValue;
  } else if (node->type == EQ_NODE_EQ) {
    Equation *subEq = getEquation(node->stringValue);
    if (subEq) {
      EqEvalData *subEvalData = NULL;
      for (int i = 0; i < evalData->subEvalDataArray->count; i++) {
        EqEvalData *possibleSubEvalData = (EqEvalData *)get(evalData->subEvalDataArray, i);
        if (possibleSubEvalData->relatedNodeId == node->id) {
          subEvalData = possibleSubEvalData;
          break;
        }
      }

      if (!subEvalData) {
        subEvalData = (EqEvalData *)getNext(evalData->subEvalDataArray); //@doubleInitEvalData I do this twice kinda
        strcpy(subEvalData->eqName, subEq->name);
        subEvalData->relatedNodeId = node->id;
        subEvalData->subEvalDataArray = createBucketArray(sizeof(EqEvalData), 16);
        subEvalData->nodeDataArray = createBucketArray(sizeof(EqNodeEvalData), 16);
      }

      // Set up input vars inside of subEvalData
			for (int i = 0; i < subEq->inputsNum; i++) {
				setVar(subEvalData, subEq->inputs[i], inputs[i]);
			}

      step(subEq, subEqDt, subEvalData);

      // Copy dest to current node outputs
			EqNode *destSubNode = getNodeOfType(subEq, EQ_NODE_DEST);
			if (destSubNode) {
				EqNodeEvalData *subNodeEvalData = getNodeEvalData(destSubNode, subEvalData);
				for (int i = 0; i < subEq->outputsNum; i++) {
					outputs[i] = subNodeEvalData->outputs[i];
				}
			} else {
				logf("Subeq %s has no dest node?\n", node->stringValue);
			}
    } else {
			logf("No subeq %s?\n", node->stringValue);
		}
  } else if (node->type == EQ_NODE_IF) {
		if (node->boolValue) {
			if (inputs[0] >= inputs[1]) {
				outputs[0] = inputs[2];
			} else {
				outputs[0] = inputs[3];
			}
		} else {
			if (inputs[0] <= inputs[1]) {
				outputs[0] = inputs[2];
			} else {
				outputs[0] = inputs[3];
			}
		}
  } else if (node->type == EQ_NODE_MODULO) {
		outputs[0] = fmod(inputs[0], inputs[1]);
  } else if (node->type == EQ_NODE_TICKER) {
		float targetValue = node->vec4Value.x;
		bool onRisingEdge = node->intValue & EQ_NODE_TICKER_ON_RISING_EDGE;
		bool onFallingEdge = node->intValue & EQ_NODE_TICKER_ON_FALLING_EDGE;

		float prevValue = nodeEvalData->prevTickerInput;
		float currentValue = inputs[0];
		if (onRisingEdge && currentValue >= targetValue && prevValue < targetValue) nodeEvalData->tickerValue++;
		if (onFallingEdge && currentValue <= targetValue && prevValue > targetValue) nodeEvalData->tickerValue++;
		nodeEvalData->prevTickerInput = currentValue;

		outputs[0] = nodeEvalData->tickerValue;
  } else if (node->type == EQ_NODE_ORDERED_RNG) {
		int steps = inputs[0];
		steps %= 100;
		pushRndSeed(nodeEvalData->rngSeed);
		for (int i = 0; i < steps-1; i++) rnd();
		outputs[0] = rndFloat(0, 1);
		popRndSeed();
  } else if (node->type == EQ_NODE_SINK) {
		// Nothing...
	} else {
		outputs[0] = 0;
	}

  //@todo Make this debug only?
  if (nodeEvalData->simValuesNum > EQ_SIM_VALUES_MAX-1) nodeEvalData->simValuesNum = 0;
  for (int i = 0; i < EQ_NODE_PINS_MAX; i++) nodeEvalData->simValues[i][nodeEvalData->simValuesNum] = outputs[i];
  nodeEvalData->simValuesNum++;

  nodeEvalData->evaled = true;
}

Equation *getEquation(char *name) {
  EquationSet *eqSet = eqSys->eqSet;

	for (int i = 0; i < eqSet->eqsNum; i++) {
		Equation *eq = &eqSet->eqs[i];
		if (streq(eq->name, name)) return eq;
  }

	logf("Couldn't find equation %s\n", name);
  return NULL;
}

EqNode *getNode(Equation *eq, s64 nodeId) {
  for (int i = 0; i < eq->nodesNum; i++) {
    EqNode *node = &eq->nodes[i];
    if (node->id == nodeId) return node;
  }
  return NULL;
}

EqEvalData *getEvalData(char *eqName, char *name) {
  EquationSet *eqSet = eqSys->eqSet;

  for (int i = 0; i < eqSys->storedEvalDatasNum; i++) {
    EqEvalData *evalData = &eqSys->storedEvalDatas[i];
    if (streq(evalData->eqName, eqName) && streq(evalData->name, name)) return evalData;
  }

  if (eqSys->storedEvalDatasNum > STORED_EVAL_DATAS_MAX-1) {
    logf("Too many stored eval datas!\n");
    eqSys->storedEvalDatasNum--;
  }

  Equation *eq = NULL;
  if (streq(eqName, "Debug")) {
    eq = &eqSet->eqs[eqSys->selectedEqIndex];
  } else {
    eq = getEquation(eqName);
  }

  EqEvalData *evalData = &eqSys->storedEvalDatas[eqSys->storedEvalDatasNum++]; //@doubleInitEvalData
  memset(evalData, 0, sizeof(EqEvalData));
  strcpy(evalData->eqName, eqName);
  strcpy(evalData->name, name);
  evalData->subEvalDataArray = createBucketArray(sizeof(EqEvalData), 16);
	evalData->nodeDataArray = createBucketArray(sizeof(EqNodeEvalData), 16);
  return evalData;
}

s64 getNodeIdThatHasInputPinId(Equation *eq, s64 inputPinId) {
	for (int i = 0; i < eq->nodesNum; i++) {
		EqNode *node = &eq->nodes[i];
		for (int i = 0; i < EQ_NODE_PINS_MAX; i++) {
			if (node->inputPinIds[i] == inputPinId) return node->id;
		}
	}
	return -1;
}

s64 getNodeIdThatHasOutputPinId(Equation *eq, s64 outputPinId) {
	for (int i = 0; i < eq->nodesNum; i++) {
		EqNode *node = &eq->nodes[i];
		for (int i = 0; i < EQ_NODE_PINS_MAX; i++) {
			if (node->outputPinIds[i] == outputPinId) return node->id;
		}
	}
	return -1;
}

s64 getNodeIdThatHasPinId(Equation *eq, s64 pinId) {
	for (int i = 0; i < eq->nodesNum; i++) {
		EqNode *node = &eq->nodes[i];
		for (int i = 0; i < EQ_NODE_PINS_MAX; i++) {
			if (node->inputPinIds[i] == pinId) return node->id;
			if (node->outputPinIds[i] == pinId) return node->id;
		}
	}
	return -1;
}

s64 isInputPin(Equation *eq, s64 pinId) {
	for (int i = 0; i < eq->nodesNum; i++) {
		EqNode *node = &eq->nodes[i];
		for (int i = 0; i < EQ_NODE_PINS_MAX; i++) {
			if (node->inputPinIds[i] == pinId) return true;
		}
	}
	return false;
}

s64 isOutputPin(Equation *eq, s64 pinId) {
	for (int i = 0; i < eq->nodesNum; i++) {
		EqNode *node = &eq->nodes[i];
		for (int i = 0; i < EQ_NODE_PINS_MAX; i++) {
			if (node->outputPinIds[i] == pinId) return true;
		}
	}
	return false;
}

EqNode *getNodeThatHasThisPinId(Equation *eq, s64 pinId, int *pinIndex) {
  for (int i = 0; i < eq->nodesNum; i++) {
    EqNode *node = &eq->nodes[i];
		for (int i = 0; i < EQ_NODE_PINS_MAX; i++) {
			if (node->inputPinIds[i] == pinId || node->outputPinIds[i] == pinId) {
        if (pinIndex) *pinIndex = i;
        return node;
      }
		}
  }
  return NULL;
}

EqNodeLink *getLinkOutputtingToThisInputPinId(Equation *eq, int pinId) {
	if (pinId == 0) return NULL;
  for (int i = 0; i < eq->linksNum; i++) {
    EqNodeLink *link = &eq->links[i];
    if (link->inputPinId == pinId) return link;
  }

  return NULL;
}

EqNodeLink *getLinkConnectedToThisPinId(Equation *eq, int pinId) {
  for (int i = 0; i < eq->linksNum; i++) {
    EqNodeLink *link = &eq->links[i];
    if (link->inputPinId == pinId) return link;
    if (link->outputPinId == pinId) return link;
  }

  return NULL;
}

void destroy(EqEvalData *evalData) {
  if (eqSys->attachedEvalData == evalData) eqSys->attachedEvalData = NULL;

  for (int i = 0; i < evalData->subEvalDataArray->count; i++) {
    EqEvalData *subData = (EqEvalData *)get(evalData->subEvalDataArray, i);
    destroy(subData);
  }

  destroy(evalData->subEvalDataArray);
  destroy(evalData->nodeDataArray);

  for (int i = 0; i < eqSys->storedEvalDatasNum; i++) {
    EqEvalData *otherEvalData = &eqSys->storedEvalDatas[i];
    if (otherEvalData == evalData) {
      arraySpliceIndex(eqSys->storedEvalDatas, eqSys->storedEvalDatasNum, sizeof(EqEvalData), i);
      eqSys->storedEvalDatasNum--;
      break;
    }
  }
}

EqEvalData *reset(EqEvalData *evalData) { //@incomplete You have to recursively reset...
  if (evalData == NULL) return NULL;

  if (eqSys->attachedEvalData == evalData) eqSys->attachedEvalData = NULL;

  char *eqName = frameStringClone(evalData->eqName);
  char *evalDataName = frameStringClone(evalData->name);

  destroy(evalData);

  return getEvalData(eqName, evalDataName);
}

float getVar(EqEvalData *evalData, char *varName) {
  for (int i = 0; i < evalData->varsNum; i++) {
    EqVar *var = &evalData->vars[i];
    if (streq(var->name, varName)) {
      return var->value;
    }
  }

	return 0;
}

void setVar(EqEvalData *evalData, char *varName, float value) {
  for (int i = 0; i < evalData->varsNum; i++) {
    EqVar *var = &evalData->vars[i];
    if (streq(var->name, varName)) {
      var->value = value;
      return;
    }
  }

  if (evalData->varsNum > EQ_EVAL_DATA_VARS_MAX-1) {
    logf("Too many eval data vars!\n");
    evalData->varsNum--;
  }

  EqVar *var = &evalData->vars[evalData->varsNum++];
  memset(var, 0, sizeof(EqVar));
  strcpy(var->name, varName);
  var->value = value;
}

float *getOutputs(EqEvalData *evalData) {
	Equation *eq = getEquation(evalData->eqName);
	if (!eq) {
		logf("Couldn't get equation %s for outputs\n", evalData);
		return (float *)frameMalloc(sizeof(float) * EQ_NODE_PINS_MAX);
	}

	EqNode *node = getNodeOfType(eq, EQ_NODE_DEST);
	if (!node) {
		logf("Failed to get outputs from %s\n", eq->name);
		return (float *)frameMalloc(sizeof(float) * EQ_NODE_PINS_MAX);
	}

	EqNodeEvalData *nodeEvalData = getNodeEvalData(node, evalData);
	return nodeEvalData->outputs;
}

Matrix3 getMatrixOutput(EqEvalData *evalData, Vec2 textureSize) {
	float *outputs = getOutputs(evalData);

	Vec2 pos = v2(outputs[0], outputs[1]);
	float rot = outputs[4];
	Vec2 scale = v2(outputs[2], outputs[3]);

	Matrix3 matrix = mat3();
	matrix.TRANSLATE(pos);
	matrix.ROTATE(rot);
	matrix.SCALE(scale);
	matrix.SCALE(textureSize);
	matrix.TRANSLATE(-v2(0.5, 0.5));
	return matrix;
}

EqNode *getNodeOfType(Equation *eq, EqNodeType nodeType, int *outIndex) {
	for (int i = 0; i < eq->nodesNum; i++) {
		EqNode *node = &eq->nodes[i];
		if (node->type == nodeType) {
			if (outIndex) *outIndex = i;
			return node;
		}
	}

	return NULL;
}

EqNodeEvalData *getNodeEvalData(EqNode *node, EqEvalData *evalData) {
	for (int i = 0; i < evalData->nodeDataArray->count; i++) {
		EqNodeEvalData *nodeEvalData = (EqNodeEvalData *)get(evalData->nodeDataArray, i);
		if (nodeEvalData->relatedNodeId == node->id) return nodeEvalData;
	}

	EqNodeEvalData *nodeEvalData = (EqNodeEvalData *)getNext(evalData->nodeDataArray);
	nodeEvalData->relatedNodeId = node->id;
	if (node->type == EQ_NODE_ORDERED_RNG) nodeEvalData->rngSeed = rndInt(0, lcgM);
	return nodeEvalData;
}

char **getFolderList(int *outFolderListNum) {
  EquationSet *eqSet = eqSys->eqSet;
	char **folderList = (char **)frameMalloc(sizeof(char *) * eqSet->eqsNum);
	int folderListNum = 0;
	for (int i = 0; i < eqSet->eqsNum; i++) {
		Equation *eq = &eqSet->eqs[i];
		bool shouldAdd = true;
		for (int i = 0; i < folderListNum; i++) {
			if (streq(folderList[i], eq->folder)) {
				shouldAdd = false;
				break;
			}
		}
		if (shouldAdd) folderList[folderListNum++] = eq->folder;
	}

	*outFolderListNum = folderListNum;
	return folderList;
}

void copyEquation(Equation *dest, Equation *src) {
	memcpy(dest, src, sizeof(Equation));
	dest->nodes = (EqNode *)zalloc(sizeof(EqNode) * src->nodesNum);
	memcpy(dest->nodes, src->nodes, sizeof(EqNode) * src->nodesNum);
	dest->links = (EqNodeLink *)zalloc(sizeof(EqNodeLink) * src->linksNum);
	memcpy(dest->links, src->links, sizeof(EqNodeLink) * src->linksNum);
}

void showNodeStyleEditor() {
  if (!ImGui::Begin("Style", NULL)) {
    ImGui::End();
    return;
  }

  auto paneWidth = ImGui::GetContentRegionAvail().x;

  auto& editorStyle = ImNode::GetStyle();
  ImGui::Text("Style buttons:");
  ImGui::TextUnformatted("Values");
  if (ImGui::Button("Reset to defaults"))
    editorStyle = ImNode::Style();
  ImGui::Text("/Style buttons");
  ImGui::Spacing();
  ImGui::DragFloat4("Node Padding", &editorStyle.NodePadding.x, 0.1f, 0.0f, 40.0f);
  ImGui::DragFloat("Node Rounding", &editorStyle.NodeRounding, 0.1f, 0.0f, 40.0f);
  ImGui::DragFloat("Node Border Width", &editorStyle.NodeBorderWidth, 0.1f, 0.0f, 15.0f);
  ImGui::DragFloat("Hovered Node Border Width", &editorStyle.HoveredNodeBorderWidth, 0.1f, 0.0f, 15.0f);
  ImGui::DragFloat("Hovered Node Border Offset", &editorStyle.HoverNodeBorderOffset, 0.1f, -40.0f, 40.0f);
  ImGui::DragFloat("Selected Node Border Width", &editorStyle.SelectedNodeBorderWidth, 0.1f, 0.0f, 15.0f);
  ImGui::DragFloat("Selected Node Border Offset", &editorStyle.SelectedNodeBorderOffset, 0.1f, -40.0f, 40.0f);
  ImGui::DragFloat("Pin Rounding", &editorStyle.PinRounding, 0.1f, 0.0f, 40.0f);
  ImGui::DragFloat("Pin Border Width", &editorStyle.PinBorderWidth, 0.1f, 0.0f, 15.0f);
  ImGui::DragFloat("Link Strength", &editorStyle.LinkStrength, 1.0f, 0.0f, 500.0f);
  //ImVec2  SourceDirection;
  //ImVec2  TargetDirection;
  ImGui::DragFloat("Scroll Duration", &editorStyle.ScrollDuration, 0.001f, 0.0f, 2.0f);
  ImGui::DragFloat("Flow Marker Distance", &editorStyle.FlowMarkerDistance, 1.0f, 1.0f, 200.0f);
  ImGui::DragFloat("Flow Speed", &editorStyle.FlowSpeed, 1.0f, 1.0f, 2000.0f);
  ImGui::DragFloat("Flow Duration", &editorStyle.FlowDuration, 0.001f, 0.0f, 5.0f);
  //ImVec2  PivotAlignment;
  //ImVec2  PivotSize;
  //ImVec2  PivotScale;
  //float   PinCorners;
  //float   PinRadius;
  //float   PinArrowSize;
  //float   PinArrowWidth;
  ImGui::DragFloat("Group Rounding", &editorStyle.GroupRounding, 0.1f, 0.0f, 40.0f);
  ImGui::DragFloat("Group Border Width", &editorStyle.GroupBorderWidth, 0.1f, 0.0f, 15.0f);

  ImGui::Separator();

  static ImGuiColorEditFlags edit_mode = ImGuiColorEditFlags_DisplayRGB;
  ImGui::Text("Color Mode:");
  ImGui::TextUnformatted("Filter Colors");
  ImGui::RadioButton("RGB", &edit_mode, ImGuiColorEditFlags_DisplayRGB);
  ImGui::RadioButton("HSV", &edit_mode, ImGuiColorEditFlags_DisplayHSV);
  ImGui::RadioButton("HEX", &edit_mode, ImGuiColorEditFlags_DisplayHex);
  ImGui::Text("/Color Mode");

  static ImGuiTextFilter filter;
  filter.Draw("##filter", paneWidth);

  ImGui::Spacing();

  ImGui::PushItemWidth(-160);
  for (int i = 0; i < ImNode::StyleColor_Count; ++i)
  {
    auto name = ImNode::GetStyleColorName((ImNode::StyleColor)i);
    if (!filter.PassFilter(name))
      continue;

    ImGui::ColorEdit4(name, &editorStyle.Colors[i].x, edit_mode);
  }
  ImGui::PopItemWidth();

  ImGui::End();
}

void guiShowEquationNodeWindow() {
	if (!eqSys->editorContext) {
		ImNode::Config config;
		config.NavigateButtonIndex = 2;
		config.SettingsFile = NULL;
		eqSys->editorContext = ImNode::CreateEditor(&config);
		ImNode::SetCurrentEditor(eqSys->editorContext);

		eqSys->timeSinceLastNodeRefresh = 0;
		eqSys->toolSplitPerc = 0.2;
		eqSys->editorAlpha = 1;

		auto& editorStyle = ImNode::GetStyle();
		editorStyle.NodeRounding = 2.5;
		editorStyle.HoveredNodeBorderWidth = 3.5;
		editorStyle.HoverNodeBorderOffset = 20;
		editorStyle.SelectedNodeBorderWidth = 10;
		editorStyle.PinBorderWidth = 5;
		editorStyle.LinkStrength = 500;

		// eqSys->windowBgColor = guiGetStyleColor(ImGuiCol_WindowBg);
		// eqSys->gridColor = fromImColor(editorStyle.Colors[ImNode::StyleColor_Grid]);
		// eqSys->bgColor = fromImColor(editorStyle.Colors[ImNode::StyleColor_Bg]);

		eqSys->windowBgColor = 0x832B2B36;
		eqSys->bgColor = 0x3C161616;
		eqSys->gridColor = 0x28D4ECFF;

		eqSys->windowBgColor = setAofArgb(eqSys->windowBgColor, getAofArgb(eqSys->windowBgColor) * 0.8);
	}

	static bool showStyleEditor = false;
	if (showStyleEditor) showNodeStyleEditor();

	int flags = ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoScrollWithMouse|ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_NoBringToFrontOnFocus;
	if (!eqSys->editorFloating) {
		flags |= ImGuiWindowFlags_NoResize;
		flags |= ImGuiWindowFlags_NoTitleBar;
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(platform->windowWidth, platform->windowHeight), ImGuiCond_Always);
	}

	if (eqSys->prevEditorFloating != eqSys->editorFloating) {
		eqSys->prevEditorFloating = eqSys->editorFloating;
		if (eqSys->editorFloating) ImGui::SetNextWindowSize(ImVec2(platform->windowWidth, platform->windowHeight)*0.5, ImGuiCond_Always);
	}

	guiPushStyleColor(ImGuiCol_WindowBg, eqSys->windowBgColor);
	ImGui::PushStyleVar(ImGuiStyleVar_Alpha, eqSys->editorAlpha);
	ImGui::Begin("Node editor", NULL, flags);

	EquationSet *eqSet = eqSys->eqSet;
	Equation *selectedEq = NULL;
	if (eqSys->selectedEqIndex < eqSet->eqsNum) selectedEq = &eqSet->eqs[eqSys->selectedEqIndex];

	auto getAllEvalDatas = [](int *outDatasNum) -> EqEvalData **{
		int allDatasNum = 0;
		EqEvalData **allDatas = (EqEvalData **)zalloc(sizeof(EqEvalData *) * eqSys->storedEvalDatasNum);
		for (int i = 0; i < eqSys->storedEvalDatasNum; i++) {
			allDatas[allDatasNum++] = &eqSys->storedEvalDatas[i];
		}

		for (int i = 0; i < allDatasNum; i++) {
			// I do this because allData[i] will be stale when new stuff is added
			BucketArray *subEvalDataArray = allDatas[i]->subEvalDataArray;

			int insertIndex = i+1;
			for (int i = 0; i < subEvalDataArray->count; i++) {
				EqEvalData *subData = (EqEvalData *)get(subEvalDataArray, i);
				allDatas = (EqEvalData **)resizeArray(allDatas, sizeof(EqEvalData *), allDatasNum, allDatasNum+1);
				allDatasNum++;
				memmove(&allDatas[insertIndex+1], &allDatas[insertIndex], sizeof(EqEvalData *) * (allDatasNum-1 - insertIndex));
				allDatas[insertIndex] = subData;
				insertIndex++;
			}
		}

		*outDatasNum = allDatasNum;
		return allDatas;
	};

	ImVec2 contentArea;
	float toolsSplitPerc = eqSys->toolSplitPerc;
	{ /// Update tools
		contentArea = ImGui::GetContentRegionAvail();
		ImGui::BeginChild("Tools child", ImVec2(contentArea.x*toolsSplitPerc, contentArea.y), true);

		if (guiColoredTreeNodeEx("Node editor", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::Checkbox("Floating", &eqSys->editorFloating);
			ImGui::InputFloat("Tool perc", &eqSys->toolSplitPerc, 0.05);
			guiInputArgb("Window color", &eqSys->windowBgColor);
			guiInputArgb("Editor bg color", &eqSys->bgColor);
			guiInputArgb("Grid color", &eqSys->gridColor);
			// ImGui::Checkbox("Show style editor", &showStyleEditor);
			guiColoredTreePop();
		}

		if (guiColoredTreeNodeEx("Equation set", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::Text("Change equation:");
			ImGui::Indent(20);

#if 1
			int folderListNum;
			char **folderList = getFolderList(&folderListNum);

			for (int i = 0; i < folderListNum; i++) {
				char *folder = folderList[i];
				int flags = 0;
				if (i == 0) flags = ImGuiTreeNodeFlags_DefaultOpen;
				if (ImGui::TreeNodeEx(frameSprintf("%s##folderTree%d\n", folder, i), flags)) {
					for (int i = 0; i < eqSet->eqsNum; i++) {
						Equation *eq = &eqSet->eqs[i];
						if (!streq(eq->folder, folder)) continue;

						ImGui::PushID(i);

						if (ImGui::Selectable(eq->name, eqSys->selectedEqIndex == i)) {
							eqSys->selectedEqIndex = i;
							if (eqSys->selectedEqIndex < eqSet->eqsNum) selectedEq = &eqSet->eqs[eqSys->selectedEqIndex];
							eqSys->timeSinceLastNodeRefresh = 0;
							eqSys->attachedEvalData = NULL;
						}

						ImGui::PopID();
					}
					ImGui::TreePop();
				}
			}

#else
			for (int i = 0; i < eqSet->eqsNum; i++) {
				ImGui::PushID(i);
				Equation *eq = &eqSet->eqs[i];

				if (keyPressed(KEY_CTRL)) {
					if (ImGui::Button("X")) {
						arraySpliceIndex(eqSet->eqs, eqSet->eqsNum, sizeof(Equation), i);
						eqSet->eqsNum--;
						if (eqSys->selectedEqIndex == i) {
							if (eqSys->selectedEqIndex < eqSet->eqsNum) selectedEq = &eqSet->eqs[eqSys->selectedEqIndex];
							eqSys->timeSinceLastNodeRefresh = 0;
							eqSys->attachedEvalData = NULL;
						}
						i--;
						ImGui::PopID();
						continue;
					}
					ImGui::SameLine();

					if (ImGui::ArrowButton("Move eq up", ImGuiDir_Up) && i > 0) {
						arraySwap(eqSet->eqs, eqSet->eqsNum, sizeof(Equation), i, i-1);
						if (eqSys->selectedEqIndex == i) eqSys->selectedEqIndex--;
						if (eqSys->selectedEqIndex < eqSet->eqsNum) selectedEq = &eqSet->eqs[eqSys->selectedEqIndex];
					}
					ImGui::SameLine();
					if (ImGui::ArrowButton("Move eq down", ImGuiDir_Down) && i < eqSet->eqsNum-1) {
						arraySwap(eqSet->eqs, eqSet->eqsNum, sizeof(Equation), i, i+1);
						if (eqSys->selectedEqIndex == i) eqSys->selectedEqIndex++;
						if (eqSys->selectedEqIndex < eqSet->eqsNum) selectedEq = &eqSet->eqs[eqSys->selectedEqIndex];
					}
					ImGui::SameLine();
				}
				if (ImGui::Selectable(eq->name, eqSys->selectedEqIndex == i)) {
					eqSys->selectedEqIndex = i;
					if (eqSys->selectedEqIndex < eqSet->eqsNum) selectedEq = &eqSet->eqs[eqSys->selectedEqIndex];
					eqSys->timeSinceLastNodeRefresh = 0;
					eqSys->attachedEvalData = NULL;
				}
				ImGui::PopID();
			}
#endif
			ImGui::Unindent(20);
			ImGui::Separator();

			if (ImGui::Button("Create new equation")) createEquation();

			if (ImGui::Button("Duplicate equation")) {
				Equation *newEq = createEquation();
				copyEquation(newEq, selectedEq);
				snprintf(newEq->name, EQUATION_NAME_MAX_LEN, "%s copy", selectedEq->name);
			}
			guiColoredTreePop();
		}

		if (guiColoredTreeNodeEx("Equation", ImGuiTreeNodeFlags_DefaultOpen)) {
			if (selectedEq) {
				ImGui::InputText("Name", selectedEq->name, EQUATION_NAME_MAX_LEN);
				ImGui::InputText("Folder", selectedEq->folder, EQUATION_NAME_MAX_LEN);

				if (ImGui::TreeNode("I/O")) {
					ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x*0.5);

					guiPushStyleColor(ImGuiCol_FrameBg, lerpColor(guiGetStyleColor(ImGuiCol_FrameBg), 0xFFFF0000, 0.25));
					for (int i = 0; i < selectedEq->inputsNum; i++) {
						ImGui::InputText(frameSprintf("input[%d]", i), selectedEq->inputs[i], EQ_NODE_STRING_VALUE_MAX_LEN);
					}
					strcpy(selectedEq->inputs[0], "t");

					ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x*0.3);
					ImGui::InputInt("Inputs", &selectedEq->inputsNum);
					selectedEq->inputsNum = mathClamp(selectedEq->inputsNum, 1, EQ_NODE_PINS_MAX-1);
					ImGui::PopItemWidth();
					guiPopStyleColor();

					ImGui::Separator();

					guiPushStyleColor(ImGuiCol_FrameBg, lerpColor(guiGetStyleColor(ImGuiCol_FrameBg), 0xFF0000FF, 0.25));
					for (int i = 0; i < selectedEq->outputsNum; i++) {
						ImGui::InputText(frameSprintf("output[%d]", i), selectedEq->outputs[i], EQ_NODE_STRING_VALUE_MAX_LEN);
					}

					ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x*0.3);
					ImGui::InputInt("Outputs", &selectedEq->outputsNum);
					selectedEq->outputsNum = mathClamp(selectedEq->outputsNum, 0, EQ_NODE_PINS_MAX-1);
					ImGui::PopItemWidth();
					guiPopStyleColor();

					ImGui::PopItemWidth();
					ImGui::TreePop();
				}

				ImGui::Text("Create:");
				if (ImGui::Button("Source")) createNode(selectedEq, EQ_NODE_SOURCE); ImGui::SameLine();
				if (ImGui::Button("Destination")) createNode(selectedEq, EQ_NODE_DEST); ImGui::SameLine();
				if (ImGui::Button("Battery")) createNode(selectedEq, EQ_NODE_BATTERY); ImGui::SameLine();
				if (ImGui::Button("Eq")) createNode(selectedEq, EQ_NODE_EQ);
				if (ImGui::Button("Ease")) createNode(selectedEq, EQ_NODE_EASE); ImGui::SameLine();
				if (ImGui::Button("Sine")) createNode(selectedEq, EQ_NODE_SINE); ImGui::SameLine();
				if (ImGui::Button("Perlin")) createNode(selectedEq, EQ_NODE_PERLIN);
				if (ImGui::Button("Split")) createNode(selectedEq, EQ_NODE_SPLIT); ImGui::SameLine();
				if (ImGui::Button("Map")) createNode(selectedEq, EQ_NODE_MAP); ImGui::SameLine();
				if (ImGui::Button("Fade")) createNode(selectedEq, EQ_NODE_FADE); ImGui::SameLine();
				if (ImGui::Button("If")) createNode(selectedEq, EQ_NODE_IF);
				if (ImGui::Button("Add")) createNode(selectedEq, EQ_NODE_ADD); ImGui::SameLine();
				if (ImGui::Button("Subtract")) createNode(selectedEq, EQ_NODE_SUBTRACT); ImGui::SameLine();
				if (ImGui::Button("Multiply")) createNode(selectedEq, EQ_NODE_MULTIPLY); ImGui::SameLine();
				if (ImGui::Button("Divide")) createNode(selectedEq, EQ_NODE_DIVIDE); ImGui::SameLine();
				if (ImGui::Button("Modulo")) createNode(selectedEq, EQ_NODE_MODULO);
				if (ImGui::Button("Ticker")) createNode(selectedEq, EQ_NODE_TICKER); ImGui::SameLine();
				if (ImGui::Button("Ordered Rng")) createNode(selectedEq, EQ_NODE_ORDERED_RNG); ImGui::SameLine();
				if (ImGui::Button("Sink")) createNode(selectedEq, EQ_NODE_SINK);
				ImGui::NewLine();
			}

			guiColoredTreePop();
		}

		if (guiColoredTreeNodeEx("Debugger", ImGuiTreeNodeFlags_DefaultOpen)) {
			if (selectedEq) {
				char *attachedName = "None";
				if (eqSys->attachedEvalData) attachedName = frameSprintf("%s|%s", eqSys->attachedEvalData->eqName, eqSys->attachedEvalData->name);
				if (ImGui::BeginCombo("Attach", attachedName, ImGuiComboFlags_HeightLarge)) {
					int allEvalDatasNum;
					EqEvalData **allEvalDatas = getAllEvalDatas(&allEvalDatasNum);
					for (int i = 0; i < allEvalDatasNum; i++) {
						EqEvalData *evalData = allEvalDatas[i];
						if (!streq(evalData->eqName, selectedEq->name) && !streq(evalData->eqName, "Debug")) continue;
						char *selectableName = frameSprintf("%s|%s###ComboItem%d", evalData->eqName, evalData->name, i);
						if (ImGui::Selectable(selectableName, eqSys->attachedEvalData == evalData)) eqSys->attachedEvalData = evalData;
					}
					ImGui::EndCombo();
					free(allEvalDatas);
				}

				if (eqSys->attachedEvalData && streq(eqSys->attachedEvalData->eqName, "Debug")) {
					ImGui::Checkbox("Sim", &eqSys->simNodes);
					ImGui::SameLine();
				}
				if (ImGui::Button("Reset")) {
					if (eqSys->attachedEvalData) eqSys->attachedEvalData = reset(eqSys->attachedEvalData);
				}
			}

			guiColoredTreePop();
		}

		if (guiColoredTreeNodeEx("Import/Export")) {
			int folderListNum;
			char **folderList = getFolderList(&folderListNum);

			if (ImGui::TreeNode("Import###ImportTreeNode")) {
				ImGui::InputText("Import path", eqSys->exportPath, PATH_MAX_LEN);
				if (ImGui::Button("Import")) {
					DataStream *stream = loadDataStream(eqSys->exportPath);
					if (stream) {
						int version = readU32(stream);
						int toLoadNum = readU32(stream);
						Equation *incomingEq = (Equation *)frameMalloc(sizeof(Equation));
						for (int i = 0; i < toLoadNum; i++) {
							saveLoadEquation(stream, false, version, incomingEq);

							Equation *existingEq = getEquation(incomingEq->name);
							if (existingEq) {
								copyEquation(existingEq, incomingEq);
								logf("Updated equation %s/%s\n", existingEq->folder, existingEq->name);
							} else {
								existingEq = createEquation();
								if (existingEq) {
									copyEquation(existingEq, incomingEq);
									logf("Imported new equation %s/%s\n", existingEq->folder, existingEq->name);
								} else {
									logf("Could not import, too many equations\n");
								}
							}

						}
					} else {
						logf("No file at path %s\n", eqSys->exportPath);
					}

					eqSys->timeSinceLastNodeRefresh = 0;
					eqSys->attachedEvalData = NULL;
				}
				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Export###ExportTreeNode")) {
				for (int i = 0; i < folderListNum; i++) {
					char *folder = folderList[i];
					ImGui::Checkbox(folder, &eqSys->foldersToExport[i]);
				}

				ImGui::InputText("Export path", eqSys->exportPath, PATH_MAX_LEN);
				if (ImGui::Button("Export")) {
					Equation **toSave = (Equation **)frameMalloc(sizeof(Equation *) * EQUATION_EXPORTS_MAX);
					int toSaveNum = 0;
					for (int i = 0; i < folderListNum; i++) {
						if (!eqSys->foldersToExport[i]) continue;

						char *folderName = folderList[i];
						for (int i = 0; i < eqSet->eqsNum; i++) {
							Equation *eq = &eqSet->eqs[i];
							if (streq(eq->folder, folderName)) {
								toSave[toSaveNum++] = eq;
							}
						}
					}

					DataStream *stream = newDataStream();
					writeU32(stream, EQUATION_SET_SAVE_VERSION);
					writeU32(stream, toSaveNum);
					for (int i = 0; i < toSaveNum; i++) {
						saveLoadEquation(stream, true, EQUATION_SET_SAVE_VERSION, toSave[i]);
					}
					writeDataStream(eqSys->exportPath, stream);
					destroyDataStream(stream);
				}
				ImGui::TreePop();
			}

			guiColoredTreePop();
		}

		ImGui::EndChild();
	} ///

	{ /// Update debugger attachment
		if (eqSys->attachedEvalData == NULL) {
			int allEvalDatasNum;
			EqEvalData **allEvalDatas = getAllEvalDatas(&allEvalDatasNum);
			for (int i = 0; i < allEvalDatasNum; i++) {
				EqEvalData *evalData = allEvalDatas[i];
				if (streq(evalData->eqName, selectedEq->name)) {
					eqSys->attachedEvalData = evalData;
					break;
				}
			}
			free(allEvalDatas);
			if (!eqSys->attachedEvalData) eqSys->attachedEvalData = reset(getEvalData("Debug", "Debug"));
		}
	}

	ImGui::SameLine();

	{ /// Update graph
		float graphSplitPerc = 1 - toolsSplitPerc;
		ImGui::BeginChild("Graph child", ImVec2(contentArea.x*graphSplitPerc, contentArea.y));

		if (selectedEq) {
			ImNode::SetCurrentEditor(eqSys->editorContext);
			ImNode::PushStyleColor(ImNode::StyleColor_Grid, toImColor(eqSys->gridColor));
			ImNode::PushStyleColor(ImNode::StyleColor_Bg, toImColor(eqSys->bgColor));
			ImNode::Begin("MyNodeEditor", ImVec2());

			for (int i = 0; i < selectedEq->nodesNum; i++) {
				EqNode *node = &selectedEq->nodes[i];

				if (eqSys->timeSinceLastNodeRefresh < 0.2) {
					ImNode::SetNodePosition(node->id, ImVec2(node->position.x, node->position.y));
				}

				char *inputs[EQ_NODE_PINS_MAX];
				int inputsNum = 0;
				char *outputs[EQ_NODE_PINS_MAX];
				int outputsNum = 0;

				if (node->type == EQ_NODE_SOURCE) {
					outputs[outputsNum++] = "t";
				} else if (node->type == EQ_NODE_DEST) {
					for (int i = 0; i < selectedEq->outputsNum; i++) {
						inputs[inputsNum++] = selectedEq->outputs[i];
					}
				} else if (node->type == EQ_NODE_EASE) {
					inputs[inputsNum++] = "in";
					outputs[outputsNum++] = "out";
				} else if (node->type == EQ_NODE_SINE) {
					inputs[inputsNum++] = "in";
					outputs[outputsNum++] = "out";
				} else if (node->type == EQ_NODE_CLAMP) {
					inputs[inputsNum++] = "in";
					outputs[outputsNum++] = "out";
				} else if (node->type == EQ_NODE_MAP) {
					inputs[inputsNum++] = "in";
					outputs[outputsNum++] = "out";
				} else if (node->type == EQ_NODE_MULTIPLY) {
					inputs[inputsNum++] = "a";
					inputs[inputsNum++] = "b";
					outputs[outputsNum++] = "out";
				} else if (node->type == EQ_NODE_ADD) {
					inputs[inputsNum++] = "a";
					inputs[inputsNum++] = "b";
					outputs[outputsNum++] = "out";
				} else if (node->type == EQ_NODE_FADE) {
					inputs[inputsNum++] = "in";
					inputs[inputsNum++] = "t";
					outputs[outputsNum++] = "out";
				} else if (node->type == EQ_NODE_SUBTRACT) {
					inputs[inputsNum++] = "a";
					inputs[inputsNum++] = "b";
					outputs[outputsNum++] = "out";
				} else if (node->type == EQ_NODE_DIVIDE) {
					inputs[inputsNum++] = "a";
					inputs[inputsNum++] = "b";
					outputs[outputsNum++] = "out";
				} else if (node->type == EQ_NODE_PERLIN) {
					inputs[inputsNum++] = "x";
					inputs[inputsNum++] = "y";
					outputs[outputsNum++] = "out";
				} else if (node->type == EQ_NODE_SPLIT) {
					if (node->intValue < 1) node->intValue = 1;
					inputs[inputsNum++] = "in";
					for (int i = 0; i < node->intValue; i++) outputs[outputsNum++] = frameSprintf("%d", i);
				} else if (node->type == EQ_NODE_BATTERY) {
					inputs[inputsNum++] = "write";
					outputs[outputsNum++] = "read";
				} else if (node->type == EQ_NODE_EQ) {
					Equation *subEq = getEquation(node->stringValue);
					if (subEq) {
						for (int i = 0; i < subEq->inputsNum; i++) {
							inputs[inputsNum++] = subEq->inputs[i];
						}
						for (int i = 0; i < subEq->outputsNum; i++) outputs[outputsNum++] = subEq->outputs[i];
					}
				} else if (node->type == EQ_NODE_IF) {
					inputs[inputsNum++] = "a";
					inputs[inputsNum++] = "b";
					inputs[inputsNum++] = "c";
					inputs[inputsNum++] = "d";
					outputs[outputsNum++] = "return";
				} else if (node->type == EQ_NODE_MODULO) {
					inputs[inputsNum++] = "a";
					inputs[inputsNum++] = "b";
					outputs[outputsNum++] = "out";
				} else if (node->type == EQ_NODE_TICKER) {
					inputs[inputsNum++] = "t";
					outputs[outputsNum++] = "count";
				} else if (node->type == EQ_NODE_ORDERED_RNG) {
					inputs[inputsNum++] = "index";
					outputs[outputsNum++] = "t";
				} else if (node->type == EQ_NODE_SINK) {
					inputs[inputsNum++] = "t";
				}

				ImNode::BeginNode(node->id);
				ImGui::Text(frameSprintf("%s", eqNodeTypeStrings[node->type]));

				{
					ImGui::BeginGroup();
					for (int i = 0; i < EQ_NODE_PINS_MAX; i++) {
						if (i < inputsNum) {
							if (getLinkConnectedToThisPinId(selectedEq, node->inputPinIds[i])) {
								ImNode::BeginPin(node->inputPinIds[i], ImNode::PinKind::Input);
								imGuiTexture(renderer->circleTexture32);
								ImGui::SameLine();
								ImGui::Text("%s", inputs[i]);
							} else {
								ImNode::BeginPin(node->inputPinIds[i], ImNode::PinKind::Input);
								imGuiTexture(renderer->circleTexture32);
								ImGui::SameLine();
								ImGui::Text("%s", inputs[i]);
								ImGui::SameLine();
								ImGui::PushItemWidth(150);
								ImGui::DragFloat(frameSprintf("###constantInput%d", node->inputPinIds[i]), &node->constantInputs[i], 0.01);
								ImGui::PopItemWidth();
							}
							ImNode::EndPin();
						} else if (getLinkConnectedToThisPinId(selectedEq, node->inputPinIds[i])) {
							ImNode::BeginPin(node->inputPinIds[i], ImNode::PinKind::Input);
							ImGui::TextColored(ImVec4(1, 0, 0, 1), "Illegal link");
							ImNode::EndPin();
						}
					}

					ImGui::EndGroup();

					ImGui::SameLine();
					ImGui::BeginGroup(); 

					for (int i = 0; i < EQ_NODE_PINS_MAX; i++) {
						char *outLabel = NULL;
						if (i < outputsNum) {
							outLabel = outputs[i];
						} else if (getLinkConnectedToThisPinId(selectedEq, node->outputPinIds[i])) {
							outLabel = "Illegal link";
						}

						if (outLabel) {
							// float spaceLeft = ImNode::GetNodeSize(node->id).x - ImGui::GetCursorPosX();
							// float textSize = ImGui::CalcTextSize(outLabel, NULL, true).x;
							// float bumpSize = spaceLeft - textSize;
							// ImGui::Dummy(ImVec2(bumpSize*0.9, 10));
							ImGui::Text("      ");
							ImGui::SameLine();
							ImNode::BeginPin(node->outputPinIds[i], ImNode::PinKind::Output);
							ImGui::Text("%s", outLabel);
							ImGui::SameLine();
							imGuiTexture(renderer->circleTexture32);
							ImNode::EndPin();
						}
					}

					ImGui::EndGroup();
				}

				{
					ImGui::PushID(i);
					ImGui::PushItemWidth(150);

					if (node->type == EQ_NODE_SOURCE) {
						ImGui::InputText("Var", node->stringValue, EQ_NODE_STRING_VALUE_MAX_LEN);
					} else if (node->type == EQ_NODE_DEST) {
						// Nothing...
					} else if (node->type == EQ_NODE_EASE) {
						if (ImGui::ArrowButton("<", ImGuiDir_Left)) node->easeType = (Ease)(node->easeType-1);
						ImGui::SameLine();
						if (ImGui::ArrowButton(">", ImGuiDir_Right)) node->easeType = (Ease)(node->easeType+1);
						ImGui::SameLine();
						node->easeType = (Ease)mathClamp(node->easeType, 0, EASES_MAX-1);
						ImGui::Text("%s", easeStrings[node->easeType]);
					} else if (node->type == EQ_NODE_SINE) {
						ImGui::DragFloat("Amplitude", &node->sineAmp, 0.01);
						ImGui::DragFloat("Frequency", &node->sineFreq, 0.01);
						ImGui::DragFloat("Phase shift", &node->sineShift, 0.01);
					} else if (node->type == EQ_NODE_CLAMP) {
						ImGui::DragFloat("Min", &node->clampMin, 0.01);
						ImGui::DragFloat("Max", &node->clampMax, 0.01);
					} else if (node->type == EQ_NODE_MAP) {
						ImGui::DragFloat2("In min/max", &node->vec4Value.x, 0.01);
						ImGui::DragFloat2("Out min/max", &node->vec4Value.z, 0.01);
						if (ImGui::ArrowButton("<", ImGuiDir_Left)) node->easeType = (Ease)(node->easeType-1);
						ImGui::SameLine();
						if (ImGui::ArrowButton(">", ImGuiDir_Right)) node->easeType = (Ease)(node->easeType+1);
						ImGui::SameLine();
						node->easeType = (Ease)mathClamp(node->easeType, 0, EASES_MAX-1);
						ImGui::Text("%s", easeStrings[node->easeType]);
						ImGui::Checkbox("Clamped", &node->boolValue);
					} else if (node->type == EQ_NODE_MULTIPLY) {
						// Nothing...
					} else if (node->type == EQ_NODE_ADD) {
						// Nothing...
					} else if (node->type == EQ_NODE_FADE) {
						ImGui::DragFloat2("start/end time", &node->vec4Value.x, 0.01);
						ImGui::DragFloat2("start/end fade", &node->vec4Value.z, 0.01);
					} else if (node->type == EQ_NODE_SUBTRACT) {
						// Nothing...
					} else if (node->type == EQ_NODE_DIVIDE) {
						// Nothing...
					} else if (node->type == EQ_NODE_PERLIN) {
						ImGui::DragFloat("Frequency", &node->floatValue, 0.01);
						ImGui::InputInt("Depth", &node->intValue);
					} else if (node->type == EQ_NODE_SPLIT) {
						ImGui::InputInt("Outputs", &node->intValue);
					} else if (node->type == EQ_NODE_BATTERY) {
						ImGui::DragFloat("Max size", &node->vec4Value.x, 0.01);
						ImGui::DragFloat("Idle decay", &node->vec4Value.y, 0.01);
					} else if (node->type == EQ_NODE_EQ) {
						ImGui::InputText("Name", node->stringValue, EQ_NODE_STRING_VALUE_MAX_LEN);

						Equation *subEq = getEquation(node->stringValue);
						if (!subEq) {
							ImGui::TextColored(ImVec4(1, 0, 0, 1), "Invalid equation");
						} else {
						}
					} else if (node->type == EQ_NODE_IF) {
						ImGui::Text("if (a");
						ImGui::SameLine();
						if (ImGui::Button(node->boolValue ? ">=" : "<=")) node->boolValue ^= 1;
						ImGui::SameLine();
						ImGui::Text("b) return c;");
						ImGui::Text("else return d;");
					} else if (node->type == EQ_NODE_MODULO) {
						// Nothing...
					} else if (node->type == EQ_NODE_TICKER) {
						ImGui::InputFloat("Target value", &node->vec4Value.x);
						ImGui::CheckboxFlags("On rising edge", &node->intValue, EQ_NODE_TICKER_ON_RISING_EDGE);
						ImGui::CheckboxFlags("On falling edge", &node->intValue, EQ_NODE_TICKER_ON_FALLING_EDGE);
					}

					int outputsToGenerate = outputsNum;
					if (node->type == EQ_NODE_DEST) outputsToGenerate = inputsNum;
					for (int i = 0; i < outputsToGenerate; i++) {
						bool showGraph = true;
						if (node->type == EQ_NODE_SPLIT) showGraph = false;
						if (!eqSys->attachedEvalData) {
							logf("It does happen\n");
							showGraph = false; // Can this even happen?
						}
						if (!showGraph) continue;

						{
							int bitmapWidth = 128;
							float *values = (float *)frameMalloc(sizeof(float) * bitmapWidth);
							float lowestValue = 999;
							float highestValue = -999;
							int graphIndex = i;

							EqNodeEvalData *nodeEvalData = getNodeEvalData(node, eqSys->attachedEvalData);
							for (int i = 0; i < EQ_SIM_VALUES_MAX; i++) {
								float value = nodeEvalData->simValues[graphIndex][i];
								if (lowestValue > value) lowestValue = value;
								if (highestValue < value) highestValue = value;
							}

							int bitmapHeight = 64;
							char *bitmapData = (char *)frameMalloc(bitmapWidth * bitmapHeight * 4);
							if (lowestValue > 0) lowestValue = 0;
							if (highestValue < 1) highestValue = 1;
							for (int y = 0; y < bitmapHeight; y++) {
								for (int x = 0; x < bitmapWidth; x++) {
									float xPerc = x / (float)bitmapWidth;
									int valueIndex = xPerc * EQ_SIM_VALUES_MAX;

									int inverseValueIndex = EQ_SIM_VALUES_MAX - valueIndex;
									valueIndex = nodeEvalData->simValuesNum - inverseValueIndex;
									while (valueIndex < 0) valueIndex += EQ_SIM_VALUES_MAX;

									float value = nodeEvalData->simValues[graphIndex][valueIndex];
									float normValue = norm(lowestValue, highestValue, value);
									float norm0 = norm(lowestValue, highestValue, 0);
									float norm1 = norm(lowestValue, highestValue, 1);

									float yPerc = y / (float)bitmapHeight;

									int color = 0xFF000000;
									if (normValue > yPerc) {
										color = 0xFF00FF00;
										if (value < 0 && yPerc < norm0) color = lerpColor(color, 0xFF0000FF, 0.8);
										if (value > 1 && yPerc > norm1) color = lerpColor(color, 0xFFFF0000, 0.8);
									}

									if (fabs(yPerc - norm0) < 0.01) color = 0xFF0000FF;
									if (fabs(yPerc - norm1) < 0.01) color = 0xFFFF0000;

									int a, r, g, b;
									hexToArgb(color, &a, &r, &g, &b);
									bitmapData[(y*bitmapWidth + x) * 4 + 0] = r;
									bitmapData[(y*bitmapWidth + x) * 4 + 1] = g;
									bitmapData[(y*bitmapWidth + x) * 4 + 2] = b;
									bitmapData[(y*bitmapWidth + x) * 4 + 3] = a;
								}
							}
							Texture *graphTexture = createFrameTexture(bitmapWidth, bitmapHeight);
							setTextureData(graphTexture, bitmapData, bitmapWidth, bitmapHeight, _F_TD_SKIP_PREMULTIPLY);
							imGuiTexture(graphTexture, v2(1, 1)*2);
						}
					}

					ImGui::PopItemWidth();
					ImGui::PopID();
				}

				ImNode::EndNode();

				ImVec2 imPosition = ImNode::GetNodePosition(node->id);
				if (eqSys->timeSinceLastNodeRefresh > 0.2) {
					node->position.x = imPosition.x;
					node->position.y = imPosition.y;
				}
			}

			for (int i = 0; i < selectedEq->linksNum; i++) {
				EqNodeLink *link = &selectedEq->links[i];
				ImNode::Link(link->id, link->inputPinId, link->outputPinId);
			}

			if (ImNode::BeginCreate()) {
				ImNode::PinId imInputPinId, imOutputPinId;
				if (ImNode::QueryNewLink(&imInputPinId, &imOutputPinId)) {

					if (imInputPinId && imOutputPinId) {
						s64 inputPinId = imInputPinId.Get();
						s64 outputPinId = imOutputPinId.Get();
						bool allowConnect = true;

						if (isInputPin(selectedEq, outputPinId) && isOutputPin(selectedEq, inputPinId)) {
							s64 temp = outputPinId;
							outputPinId = inputPinId;
							inputPinId = temp;
						}

						if (getLinkConnectedToThisPinId(selectedEq, inputPinId)) allowConnect = false;
						if (getLinkConnectedToThisPinId(selectedEq, outputPinId)) allowConnect = false;

						if (isInputPin(selectedEq, inputPinId) && isInputPin(selectedEq, outputPinId)) allowConnect = false;
						if (isOutputPin(selectedEq, inputPinId) && isOutputPin(selectedEq, outputPinId)) allowConnect = false;

						if (getNodeIdThatHasPinId(selectedEq, inputPinId) == getNodeIdThatHasPinId(selectedEq, outputPinId)) allowConnect = false;

						if (allowConnect) {
							if (ImNode::AcceptNewItem()) {
								EqNodeLink *link = createLink(selectedEq, inputPinId, outputPinId);
								ImNode::Link(link->id, link->inputPinId, link->outputPinId);
							}
						} else {
							ImNode::RejectNewItem();
						}
					}
				}
			}
			ImNode::EndCreate();

			if (ImNode::BeginDelete()) {
				ImNode::LinkId imDeletedLinkId;
				while (ImNode::QueryDeletedLink(&imDeletedLinkId)) {
					bool allowDisconnect = true;
					if (allowDisconnect) {
						if (ImNode::AcceptDeletedItem()) {
							for (int i = 0; i < selectedEq->linksNum; i++) {
								EqNodeLink *link = &selectedEq->links[i];
								if (link->id == imDeletedLinkId.Get()) {
									if (eqSys->copiedNodesNum > 0) {
										logf("Clipboard of %d nodes has been cleared\n", eqSys->copiedNodesNum);
										eqSys->copiedNodesNum = 0;
									}

									arraySpliceIndex(selectedEq->links, selectedEq->linksNum, sizeof(EqNodeLink), i);
									selectedEq->linksNum--;
									break;
								}
							}
						}
					} else {
						ImNode::RejectDeletedItem();
					}
				}

				ImNode::NodeId imDeletedNodeId;
				while (ImNode::QueryDeletedNode(&imDeletedNodeId)) {
					if (ImNode::AcceptDeletedItem()) {
						u64 deletedNodeId = imDeletedNodeId.Get();
						for (int i = 0; i < selectedEq->nodesNum; i++) {
							EqNode *node = &selectedEq->nodes[i];
							if (node->id == deletedNodeId) {
								for (int i = 0; i < selectedEq->linksNum; i++) {
									EqNodeLink *link = &selectedEq->links[i];
									bool shouldDeleteLink = false;
									for (int i = 0; i < EQ_NODE_PINS_MAX; i++) {
										if (link->inputPinId == node->inputPinIds[i]) shouldDeleteLink = true;
										if (link->outputPinId == node->inputPinIds[i]) shouldDeleteLink = true;
										if (link->inputPinId == node->outputPinIds[i]) shouldDeleteLink = true;
										if (link->outputPinId == node->outputPinIds[i]) shouldDeleteLink = true;
									}

									if (shouldDeleteLink) {
										arraySpliceIndex(selectedEq->links, selectedEq->linksNum, sizeof(EqNodeLink), i);
										selectedEq->linksNum--;
										i--;
										continue;
									}
								}

								if (eqSys->copiedNodesNum > 0) {
									logf("Clipboard of %d nodes has been cleared\n", eqSys->copiedNodesNum);
									eqSys->copiedNodesNum = 0;
								}

								arraySpliceIndex(selectedEq->nodes, selectedEq->nodesNum, sizeof(EqNode), i);
								selectedEq->nodesNum--;
								i--;
								break;
							}
						}
					}
				}
			}
			ImNode::EndDelete();

			{ /// Shortcuts
				if (keyPressed(KEY_CTRL) && keyJustPressed('C')) {
					ImNode::NodeId *selectedNodeIds = (ImNode::NodeId *)frameMalloc(sizeof(ImNode::NodeId) * selectedEq->nodesNum);
					int selectedNodeIdsNum = GetSelectedNodes(selectedNodeIds, selectedEq->nodesNum);

					if (selectedNodeIdsNum > 0) {
						if (eqSys->copiedNodes) free(eqSys->copiedNodes);
						eqSys->copiedNodes = NULL;

						eqSys->copiedNodes = (int *)malloc(sizeof(int) * selectedNodeIdsNum);
						eqSys->copiedNodesNum = 0;
						eqSys->copiedNodesEq = selectedEq;

						for (int i = 0; i < selectedNodeIdsNum; i++) {
							EqNode *node = getNode(selectedEq, selectedNodeIds[i].Get());
							if (node->type == EQ_NODE_DEST) {
								logf("Can't copy destination nodes\n");
								continue;
							}
							eqSys->copiedNodes[eqSys->copiedNodesNum++] = node->id;
						}
						logf("Copied %d nodes\n", eqSys->copiedNodesNum);
					}
				}

				if (keyPressed(KEY_CTRL) && keyJustPressed('V')) {
					if (eqSys->copiedNodesNum > 0) {
						ImNode::ClearSelection();

						for (int i = 0; i < eqSys->copiedNodesNum; i++) {
							EqNode *existingNode = getNode(eqSys->copiedNodesEq, eqSys->copiedNodes[i]);
							EqNode *newNode = createNode(selectedEq, existingNode->type);

							int newNodeId = newNode->id;
							s64 *newInputPinIds = (s64 *)frameMalloc(sizeof(s64) * EQ_NODE_PINS_MAX);
							memcpy(newInputPinIds, newNode->inputPinIds, sizeof(s64) * EQ_NODE_PINS_MAX);
							s64 *newOutputPinIds = (s64 *)frameMalloc(sizeof(s64) * EQ_NODE_PINS_MAX);
							memcpy(newOutputPinIds, newNode->outputPinIds, sizeof(s64) * EQ_NODE_PINS_MAX);

							memcpy(newNode, existingNode, sizeof(EqNode));
							newNode->id = newNodeId;
							memcpy(newNode->inputPinIds, newInputPinIds, sizeof(s64) * EQ_NODE_PINS_MAX);
							memcpy(newNode->outputPinIds, newOutputPinIds, sizeof(s64) * EQ_NODE_PINS_MAX);

							newNode->position.x += 30;
							newNode->position.y += 30;

							ImNode::BeginNode(newNode->id);
							ImNode::EndNode();
							ImNode::SelectNode(newNode->id, true);
						}

						EqNode **newNodes = (EqNode **)frameMalloc(sizeof(EqNode *) * eqSys->copiedNodesNum);
						int newNodesNum = 0;
						for (int i = selectedEq->nodesNum - eqSys->copiedNodesNum; i < selectedEq->nodesNum; i++) {
							EqNode *newNode = &selectedEq->nodes[i];
							newNodes[newNodesNum++] = newNode;
						}

						auto getNodeIndexInCopiedList = [](Equation *eq, int existingNodeId) -> int {
							for (int i = 0; i < eqSys->copiedNodesNum; i++) {
								EqNode *existingNode = getNode(eq, eqSys->copiedNodes[i]);
								if (existingNode->id == existingNodeId) return i;
							}
							return -1;
						};

						for (int i = 0; i < eqSys->copiedNodesNum; i++) {
							EqNode *existingNode = getNode(eqSys->copiedNodesEq, eqSys->copiedNodes[i]);
							EqNode *newNode = newNodes[i];
							for (int i = 0; i < EQ_NODE_PINS_MAX; i++) {
								EqNodeLink *link = getLinkConnectedToThisPinId(eqSys->copiedNodesEq, existingNode->inputPinIds[i]);
								if (!link) continue;
								int otherNodePinIndex;
								EqNode *otherExistingNode = getNodeThatHasThisPinId(eqSys->copiedNodesEq, link->outputPinId, &otherNodePinIndex);
								if (otherExistingNode->id == existingNode->id) logf("You did it wrong...\n");
								int otherNodeIndex = getNodeIndexInCopiedList(eqSys->copiedNodesEq, otherExistingNode->id);
								if (otherNodeIndex == -1) continue;

								EqNode *otherNewNode = newNodes[otherNodeIndex];
								createLink(selectedEq, newNode->inputPinIds[i], otherNewNode->outputPinIds[otherNodePinIndex]);

							}
						}

						eqSys->timeSinceLastNodeRefresh = 0;
					}
				}
			} ///

			ImNode::End();
			ImNode::PopStyleColor(2);

			ImNode::SetCurrentEditor(NULL);

			eqSys->timeSinceLastNodeRefresh += 1/60.0;
		}

		ImGui::EndChild();
	} ///

	ImGui::End();
	guiPopStyleColor();
  ImGui::PopStyleVar();
}

void updateEquationSystem(float elapsed) {
  EquationSet *eqSet = eqSys->eqSet;

  Equation *selectedEq = NULL;
  if (eqSys->selectedEqIndex < eqSet->eqsNum) selectedEq = &eqSet->eqs[eqSys->selectedEqIndex];
  if (eqSys->simNodes) {
    step(selectedEq, elapsed, getEvalData("Debug", "Debug"));
  }
}

void saveLoadEquation(DataStream *stream, bool save, int version, Equation *eq) {
	saveLoadStringInto(stream, save, version, eq->name, EQUATION_NAME_MAX_LEN, 0, 999);
	saveLoadStringInto(stream, save, version, eq->folder, EQUATION_NAME_MAX_LEN, 18, 999);

	saveLoadInt(stream, save, version, &eq->nodesNum, 0, 999);
	if (!save) eq->nodes = (EqNode *)zalloc(sizeof(EqNode) * eq->nodesNum);
	for (int i = 0; i < eq->nodesNum; i++) {
		EqNode *node = &eq->nodes[i];
		saveLoadS64(stream, save, version, &node->id, 0, 999);

		saveLoadInt(stream, save, version, (int *)&node->type, 0, 999);
		saveLoadInt(stream, save, version, (int *)&node->easeType, 2, 999);
		saveLoadInt(stream, save, version, (int *)&node->timeType, 13, 999);
		saveLoadFloat(stream, save, version, &node->sineAmp, 4, 999);
		saveLoadFloat(stream, save, version, &node->sineFreq, 4, 999);
		saveLoadFloat(stream, save, version, &node->sineShift, 4, 999);
		saveLoadFloat(stream, save, version, &node->clampMin, 3, 999);
		saveLoadFloat(stream, save, version, &node->clampMax, 3, 999);
		saveLoadVec4(stream, save, version, &node->vec4Value, 6, 999);
		saveLoadBool(stream, save, version, &node->boolValue, 7, 999);
		saveLoadFloat(stream, save, version, &node->floatValue, 6, 999);
		saveLoadInt(stream, save, version, &node->intValue, 8, 999);
		saveLoadStringInto(stream, save, version, node->stringValue, EQ_NODE_STRING_VALUE_MAX_LEN, 14, 999);
		for (int i = 0; i < EQ_NODE_PINS_MAX; i++) saveLoadS64(stream, save, version, &node->inputPinIds[i], 0, 999);
		for (int i = 0; i < EQ_NODE_PINS_MAX; i++) saveLoadS64(stream, save, version, &node->outputPinIds[i], 0, 999);
		for (int i = 0; i < EQ_NODE_PINS_MAX; i++) saveLoadFloat(stream, save, version, &node->constantInputs[i], 9, 999);

		saveLoadVec2(stream, save, version, &node->position, 0, 999);
	}

	saveLoadInt(stream, save, version, &eq->linksNum, 0, 999);
	if (!save) eq->links = (EqNodeLink *)zalloc(sizeof(EqNodeLink) * eq->linksNum);
	for (int i = 0; i < eq->linksNum; i++) {
		EqNodeLink *link = &eq->links[i];
		saveLoadS64(stream, save, version, &link->id, 0, 999);
		saveLoadS64(stream, save, version, &link->inputPinId, 0, 999);
		saveLoadS64(stream, save, version, &link->outputPinId, 0, 999);
	}

	saveLoadS64(stream, save, version, &eq->nextNodeId, 0, 999);
	saveLoadInt(stream, save, version, &eq->inputsNum, 16, 999);
	for (int i = 0; i < EQ_NODE_PINS_MAX; i++) {
		saveLoadStringInto(stream, save, version, eq->inputs[i], EQ_NODE_STRING_VALUE_MAX_LEN, 16, 999);
	}
	saveLoadInt(stream, save, version, &eq->outputsNum, 16, 999);
	for (int i = 0; i < EQ_NODE_PINS_MAX; i++) {
		saveLoadStringInto(stream, save, version, eq->outputs[i], EQ_NODE_STRING_VALUE_MAX_LEN, 16, 999);
	}
}

bool saveLoadEquationSet(DataStream *stream, bool save, int outerVersion, EquationSet *eqSet, int startVersion, int endVersion) {
  checkEquationSystemInit();
  if (!saveLoadVersionCheck(outerVersion, startVersion, endVersion, save)) return false;

  int version = EQUATION_SET_SAVE_VERSION;
  saveLoadInt(stream, save, version, &version, 0, 999);

  saveLoadInt(stream, save, version, &eqSet->eqsNum, 0, 999);
  if (!save) eqSet->eqs = (Equation *)zalloc(sizeof(Equation) * eqSet->eqsNum);
  for (int i = 0; i < eqSet->eqsNum; i++) {
		saveLoadEquation(stream, save, version, &eqSet->eqs[i]);
  }

	eqSys->timeSinceLastNodeRefresh = 0;

  return true;
}
