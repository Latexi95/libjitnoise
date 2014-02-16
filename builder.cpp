#include "builder.h"
#include <vector>
#include "llvm.h"

namespace jitnoise {
Builder::Builder(const jitnoise::Context &context) :
	mBuilderContext(std::make_shared<Builder::BuilderContext>(context)),
	mIRBuilder(0),
	mFunction(0)
{

}

Builder::Builder(std::shared_ptr<BuilderContext> context) :
	mBuilderContext(context),
	mIRBuilder(0),
	mFunction(0)
{

}

const int gRandomVectorCount = 256;
const double gRandomVectors[gRandomVectorCount * 4] = {
	-0.763874, -0.596439, -0.246489, 0.0,
	0.396055, 0.904518, -0.158073, 0.0,
	-0.499004, -0.8665, -0.0131631, 0.0,
	0.468724, -0.824756, 0.316346, 0.0,
	0.829598, 0.43195, 0.353816, 0.0,
	-0.454473, 0.629497, -0.630228, 0.0,
	-0.162349, -0.869962, -0.465628, 0.0,
	0.932805, 0.253451, 0.256198, 0.0,
	-0.345419, 0.927299, -0.144227, 0.0,
	-0.715026, -0.293698, -0.634413, 0.0,
	-0.245997, 0.717467, -0.651711, 0.0,
	-0.967409, -0.250435, -0.037451, 0.0,
	0.901729, 0.397108, -0.170852, 0.0,
	0.892657, -0.0720622, -0.444938, 0.0,
	0.0260084, -0.0361701, 0.999007, 0.0,
	0.949107, -0.19486, 0.247439, 0.0,
	0.471803, -0.807064, -0.355036, 0.0,
	0.879737, 0.141845, 0.453809, 0.0,
	0.570747, 0.696415, 0.435033, 0.0,
	-0.141751, -0.988233, -0.0574584, 0.0,
	-0.58219, -0.0303005, 0.812488, 0.0,
	-0.60922, 0.239482, -0.755975, 0.0,
	0.299394, -0.197066, -0.933557, 0.0,
	-0.851615, -0.220702, -0.47544, 0.0,
	0.848886, 0.341829, -0.403169, 0.0,
	-0.156129, -0.687241, 0.709453, 0.0,
	-0.665651, 0.626724, 0.405124, 0.0,
	0.595914, -0.674582, 0.43569, 0.0,
	0.171025, -0.509292, 0.843428, 0.0,
	0.78605, 0.536414, -0.307222, 0.0,
	0.18905, -0.791613, 0.581042, 0.0,
	-0.294916, 0.844994, 0.446105, 0.0,
	0.342031, -0.58736, -0.7335, 0.0,
	0.57155, 0.7869, 0.232635, 0.0,
	0.885026, -0.408223, 0.223791, 0.0,
	-0.789518, 0.571645, 0.223347, 0.0,
	0.774571, 0.31566, 0.548087, 0.0,
	-0.79695, -0.0433603, -0.602487, 0.0,
	-0.142425, -0.473249, -0.869339, 0.0,
	-0.0698838, 0.170442, 0.982886, 0.0,
	0.687815, -0.484748, 0.540306, 0.0,
	0.543703, -0.534446, -0.647112, 0.0,
	0.97186, 0.184391, -0.146588, 0.0,
	0.707084, 0.485713, -0.513921, 0.0,
	0.942302, 0.331945, 0.043348, 0.0,
	0.499084, 0.599922, 0.625307, 0.0,
	-0.289203, 0.211107, 0.9337, 0.0,
	0.412433, -0.71667, -0.56239, 0.0,
	0.87721, -0.082816, 0.47291, 0.0,
	-0.420685, -0.214278, 0.881538, 0.0,
	0.752558, -0.0391579, 0.657361, 0.0,
	0.0765725, -0.996789, 0.0234082, 0.0,
	-0.544312, -0.309435, -0.779727, 0.0,
	-0.455358, -0.415572, 0.787368, 0.0,
	-0.874586, 0.483746, 0.0330131, 0.0,
	0.245172, -0.0838623, 0.965846, 0.0,
	0.382293, -0.432813, 0.81641, 0.0,
	-0.287735, -0.905514, 0.311853, 0.0,
	-0.667704, 0.704955, -0.239186, 0.0,
	0.717885, -0.464002, -0.518983, 0.0,
	0.976342, -0.214895, 0.0240053, 0.0,
	-0.0733096, -0.921136, 0.382276, 0.0,
	-0.986284, 0.151224, -0.0661379, 0.0,
	-0.899319, -0.429671, 0.0812908, 0.0,
	0.652102, -0.724625, 0.222893, 0.0,
	0.203761, 0.458023, -0.865272, 0.0,
	-0.030396, 0.698724, -0.714745, 0.0,
	-0.460232, 0.839138, 0.289887, 0.0,
	-0.0898602, 0.837894, 0.538386, 0.0,
	-0.731595, 0.0793784, 0.677102, 0.0,
	-0.447236, -0.788397, 0.422386, 0.0,
	0.186481, 0.645855, -0.740335, 0.0,
	-0.259006, 0.935463, 0.240467, 0.0,
	0.445839, 0.819655, -0.359712, 0.0,
	0.349962, 0.755022, -0.554499, 0.0,
	-0.997078, -0.0359577, 0.0673977, 0.0,
	-0.431163, -0.147516, -0.890133, 0.0,
	0.299648, -0.63914, 0.708316, 0.0,
	0.397043, 0.566526, -0.722084, 0.0,
	-0.502489, 0.438308, -0.745246, 0.0,
	0.0687235, 0.354097, 0.93268, 0.0,
	-0.0476651, -0.462597, 0.885286, 0.0,
	-0.221934, 0.900739, -0.373383, 0.0,
	-0.956107, -0.225676, 0.186893, 0.0,
	-0.187627, 0.391487, -0.900852, 0.0,
	-0.224209, -0.315405, 0.92209, 0.0,
	-0.730807, -0.537068, 0.421283, 0.0,
	-0.0353135, -0.816748, 0.575913, 0.0,
	-0.941391, 0.176991, -0.287153, 0.0,
	-0.154174, 0.390458, 0.90762, 0.0,
	-0.283847, 0.533842, 0.796519, 0.0,
	-0.482737, -0.850448, 0.209052, 0.0,
	-0.649175, 0.477748, 0.591886, 0.0,
	0.885373, -0.405387, -0.227543, 0.0,
	-0.147261, 0.181623, -0.972279, 0.0,
	0.0959236, -0.115847, -0.988624, 0.0,
	-0.89724, -0.191348, 0.397928, 0.0,
	0.903553, -0.428461, -0.00350461, 0.0,
	0.849072, -0.295807, -0.437693, 0.0,
	0.65551, 0.741754, -0.141804, 0.0,
	0.61598, -0.178669, 0.767232, 0.0,
	0.0112967, 0.932256, -0.361623, 0.0,
	-0.793031, 0.258012, 0.551845, 0.0,
	0.421933, 0.454311, 0.784585, 0.0,
	-0.319993, 0.0401618, -0.946568, 0.0,
	-0.81571, 0.551307, -0.175151, 0.0,
	-0.377644, 0.00322313, 0.925945, 0.0,
	0.129759, -0.666581, -0.734052, 0.0,
	0.601901, -0.654237, -0.457919, 0.0,
	-0.927463, -0.0343576, -0.372334, 0.0,
	-0.438663, -0.868301, -0.231578, 0.0,
	-0.648845, -0.749138, -0.133387, 0.0,
	0.507393, -0.588294, 0.629653, 0.0,
	0.726958, 0.623665, 0.287358, 0.0,
	0.411159, 0.367614, -0.834151, 0.0,
	0.806333, 0.585117, -0.0864016, 0.0,
	0.263935, -0.880876, 0.392932, 0.0,
	0.421546, -0.201336, 0.884174, 0.0,
	-0.683198, -0.569557, -0.456996, 0.0,
	-0.117116, -0.0406654, -0.992285, 0.0,
	-0.643679, -0.109196, -0.757465, 0.0,
	-0.561559, -0.62989, 0.536554, 0.0,
	0.0628422, 0.104677, -0.992519, 0.0,
	0.480759, -0.2867, -0.828658, 0.0,
	-0.228559, -0.228965, -0.946222, 0.0,
	-0.10194, -0.65706, -0.746914, 0.0,
	0.0689193, -0.678236, 0.731605, 0.0,
	0.401019, -0.754026, 0.52022, 0.0,
	-0.742141, 0.547083, -0.387203, 0.0,
	-0.00210603, -0.796417, -0.604745, 0.0,
	0.296725, -0.409909, -0.862513, 0.0,
	-0.260932, -0.798201, 0.542945, 0.0,
	-0.641628, 0.742379, 0.192838, 0.0,
	-0.186009, -0.101514, 0.97729, 0.0,
	0.106711, -0.962067, 0.251079, 0.0,
	-0.743499, 0.30988, -0.592607, 0.0,
	-0.795853, -0.605066, -0.0226607, 0.0,
	-0.828661, -0.419471, -0.370628, 0.0,
	0.0847218, -0.489815, -0.8677, 0.0,
	-0.381405, 0.788019, -0.483276, 0.0,
	0.282042, -0.953394, 0.107205, 0.0,
	0.530774, 0.847413, 0.0130696, 0.0,
	0.0515397, 0.922524, 0.382484, 0.0,
	-0.631467, -0.709046, 0.313852, 0.0,
	0.688248, 0.517273, 0.508668, 0.0,
	0.646689, -0.333782, -0.685845, 0.0,
	-0.932528, -0.247532, -0.262906, 0.0,
	0.630609, 0.68757, -0.359973, 0.0,
	0.577805, -0.394189, 0.714673, 0.0,
	-0.887833, -0.437301, -0.14325, 0.0,
	0.690982, 0.174003, 0.701617, 0.0,
	-0.866701, 0.0118182, 0.498689, 0.0,
	-0.482876, 0.727143, 0.487949, 0.0,
	-0.577567, 0.682593, -0.447752, 0.0,
	0.373768, 0.0982991, 0.922299, 0.0,
	0.170744, 0.964243, -0.202687, 0.0,
	0.993654, -0.035791, -0.106632, 0.0,
	0.587065, 0.4143, -0.695493, 0.0,
	-0.396509, 0.26509, -0.878924, 0.0,
	-0.0866853, 0.83553, -0.542563, 0.0,
	0.923193, 0.133398, -0.360443, 0.0,
	0.00379108, -0.258618, 0.965972, 0.0,
	0.239144, 0.245154, -0.939526, 0.0,
	0.758731, -0.555871, 0.33961, 0.0,
	0.295355, 0.309513, 0.903862, 0.0,
	0.0531222, -0.91003, -0.411124, 0.0,
	0.270452, 0.0229439, -0.96246, 0.0,
	0.563634, 0.0324352, 0.825387, 0.0,
	0.156326, 0.147392, 0.976646, 0.0,
	-0.0410141, 0.981824, 0.185309, 0.0,
	-0.385562, -0.576343, -0.720535, 0.0,
	0.388281, 0.904441, 0.176702, 0.0,
	0.945561, -0.192859, -0.262146, 0.0,
	0.844504, 0.520193, 0.127325, 0.0,
	0.0330893, 0.999121, -0.0257505, 0.0,
	-0.592616, -0.482475, -0.644999, 0.0,
	0.539471, 0.631024, -0.557476, 0.0,
	0.655851, -0.027319, -0.754396, 0.0,
	0.274465, 0.887659, 0.369772, 0.0,
	-0.123419, 0.975177, -0.183842, 0.0,
	-0.223429, 0.708045, 0.66989, 0.0,
	-0.908654, 0.196302, 0.368528, 0.0,
	-0.95759, -0.00863708, 0.288005, 0.0,
	0.960535, 0.030592, 0.276472, 0.0,
	-0.413146, 0.907537, 0.0754161, 0.0,
	-0.847992, 0.350849, -0.397259, 0.0,
	0.614736, 0.395841, 0.68221, 0.0,
	-0.503504, -0.666128, -0.550234, 0.0,
	-0.268833, -0.738524, -0.618314, 0.0,
	0.792737, -0.60001, -0.107502, 0.0,
	-0.637582, 0.508144, -0.579032, 0.0,
	0.750105, 0.282165, -0.598101, 0.0,
	-0.351199, -0.392294, -0.850155, 0.0,
	0.250126, -0.960993, -0.118025, 0.0,
	-0.732341, 0.680909, -0.0063274, 0.0,
	-0.760674, -0.141009, 0.633634, 0.0,
	0.222823, -0.304012, 0.926243, 0.0,
	0.209178, 0.505671, 0.836984, 0.0,
	0.757914, -0.56629, -0.323857, 0.0,
	-0.782926, -0.339196, 0.52151, 0.0,
	-0.462952, 0.585565, 0.665424, 0.0,
	0.61879, 0.194119, -0.761194, 0.0,
	0.741388, -0.276743, 0.611357, 0.0,
	0.707571, 0.702621, 0.0752872, 0.0,
	0.156562, 0.819977, 0.550569, 0.0,
	-0.793606, 0.440216, 0.42, 0.0,
	0.234547, 0.885309, -0.401517, 0.0,
	0.132598, 0.80115, -0.58359, 0.0,
	-0.377899, -0.639179, 0.669808, 0.0,
	-0.865993, -0.396465, 0.304748, 0.0,
	-0.624815, -0.44283, 0.643046, 0.0,
	-0.485705, 0.825614, -0.287146, 0.0,
	-0.971788, 0.175535, 0.157529, 0.0,
	-0.456027, 0.392629, 0.798675, 0.0,
	-0.0104443, 0.521623, -0.853112, 0.0,
	-0.660575, -0.74519, 0.091282, 0.0,
	-0.0157698, -0.307475, -0.951425, 0.0,
	-0.603467, -0.250192, 0.757121, 0.0,
	0.506876, 0.25006, 0.824952, 0.0,
	0.255404, 0.966794, 0.00884498, 0.0,
	0.466764, -0.874228, -0.133625, 0.0,
	0.475077, -0.0682351, -0.877295, 0.0,
	-0.224967, -0.938972, -0.260233, 0.0,
	-0.377929, -0.814757, -0.439705, 0.0,
	-0.305847, 0.542333, -0.782517, 0.0,
	0.26658, -0.902905, -0.337191, 0.0,
	0.0275773, 0.322158, -0.946284, 0.0,
	0.0185422, 0.716349, 0.697496, 0.0,
	-0.20483, 0.978416, 0.0273371, 0.0,
	-0.898276, 0.373969, 0.230752, 0.0,
	-0.00909378, 0.546594, 0.837349, 0.0,
	0.6602, -0.751089, 0.000959236, 0.0,
	0.855301, -0.303056, 0.420259, 0.0,
	0.797138, 0.0623013, -0.600574, 0.0,
	0.48947, -0.866813, 0.0951509, 0.0,
	0.251142, 0.674531, 0.694216, 0.0,
	-0.578422, -0.737373, -0.348867, 0.0,
	-0.254689, -0.514807, 0.818601, 0.0,
	0.374972, 0.761612, 0.528529, 0.0,
	0.640303, -0.734271, -0.225517, 0.0,
	-0.638076, 0.285527, 0.715075, 0.0,
	0.772956, -0.15984, -0.613995, 0.0,
	0.798217, -0.590628, 0.118356, 0.0,
	-0.986276, -0.0578337, -0.154644, 0.0,
	-0.312988, -0.94549, 0.0899272, 0.0,
	-0.497338, 0.178325, 0.849032, 0.0,
	-0.101136, -0.981014, 0.165477, 0.0,
	-0.521688, 0.0553434, -0.851339, 0.0,
	-0.786182, -0.583814, 0.202678, 0.0,
	-0.565191, 0.821858, -0.0714658, 0.0,
	0.437895, 0.152598, -0.885981, 0.0,
	-0.92394, 0.353436, -0.14635, 0.0,
	0.212189, -0.815162, -0.538969, 0.0,
	-0.859262, 0.143405, -0.491024, 0.0,
	0.991353, 0.112814, 0.0670273, 0.0,
	0.0337884, -0.979891, -0.196654, 0.0
};

void Builder::initialize(llvm::Module *module) {
	mBuilderContext->mModule = module;

	mBuilderContext->mFPM = new llvm::FunctionPassManager(module);

	// Set up the optimizer pipeline.  Start with registering info about how the
	// target lays out data structures.
	mBuilderContext->mFPM->add(new llvm::DataLayout(module->getDataLayout()));
	// Provide basic AliasAnalysis support for GVN.
	mBuilderContext->mFPM->add(llvm::createBasicAliasAnalysisPass());
	// Do simple "peephole" optimizations and bit-twiddling optzns.
	mBuilderContext->mFPM->add(llvm::createInstructionCombiningPass());
	// Reassociate expressions.
	mBuilderContext->mFPM->add(llvm::createReassociatePass());
	// Eliminate Common SubExpressions.
	mBuilderContext->mFPM->add(llvm::createGVNPass());
	// Simplify the control flow graph (deleting unreachable blocks, etc).
	mBuilderContext->mFPM->add(llvm::createCFGSimplificationPass());

	generateDebugPrinterFunction(module);
	createRandomVectors(module);
	defineStandardFunctions(module);
	generateValueNoise3DFunction(module);
	generateIntMake32RangeFunction(module);
	generateGradientNoise3DFunction(module);
	generateGradientCoherentNoise3DFunction(module);
}

void Builder::begin(llvm::Function *func, llvm::BasicBlock *basicBlock) {
	mFunction = func;
	delete mIRBuilder;
	mIRBuilder = new llvm::IRBuilder<>(basicBlock);
}

void Builder::createRandomVectors(llvm::Module *module) {
	std::vector<llvm::Constant*> vectors(gRandomVectorCount * 4, 0);
	llvm::Type *fpType = getSingleFPType();
	for (int i = 0; i < gRandomVectorCount; ++i) {
		vectors[i * 4] = llvm::ConstantFP::get(fpType, gRandomVectors[i * 4]);
		vectors[i * 4 + 1] = llvm::ConstantFP::get(fpType, gRandomVectors[i * 4 + 1]);
		vectors[i * 4 + 2] = llvm::ConstantFP::get(fpType, gRandomVectors[i * 4 + 2]);
		vectors[i * 4 + 3] = llvm::ConstantFP::get(fpType, gRandomVectors[i * 4 + 3]);
	}
	llvm::Constant *initializer = llvm::ConstantArray::get(llvm::ArrayType::get(getSingleFPType(), gRandomVectorCount * 4), vectors);
	mBuilderContext->mRandomVectors = new llvm::GlobalVariable(*module, llvm::ArrayType::get(getSingleFPType(), gRandomVectorCount * 4), true, llvm::GlobalVariable::PrivateLinkage, initializer, "randomVectors");
}

llvm::Constant *Builder::getFPValue(double value) const {
	llvm::Type *fpType = getSingleFPType();

	if (mBuilderContext->mContext.mVectorize) {
		return llvm::ConstantVector::get(std::vector<llvm::Constant*>(4, llvm::ConstantFP::get(fpType, value)));
	}

	return llvm::ConstantFP::get(fpType, value);
}

llvm::Constant *Builder::getIntValue(int val) const {
	if (mBuilderContext->mContext.mVectorize) {
		return llvm::ConstantVector::get(std::vector<llvm::Constant*>(4, llvm::ConstantInt::get(getSingleIntType(), (int64_t)val, true)));
	}
	return  llvm::ConstantInt::get(getSingleIntType(), (int64_t)val, true);
}

const int X_NOISE_GEN = 1619;
const int Y_NOISE_GEN = 31337;
const int Z_NOISE_GEN = 6971;
const int SEED_NOISE_GEN = 1013;
const int SHIFT_NOISE_GEN = 8;


llvm::Type *Builder::getFPType() const {
	llvm::Type *fpType = getSingleFPType();
	if (mBuilderContext->mContext.mVectorize) {
		return llvm::VectorType::get(fpType, 4);
	}
	return fpType;
}

llvm::Type *Builder::getIntType() const {
	llvm::Type *intType = getSingleIntType();
	if (mBuilderContext->mContext.mVectorize) {
		return llvm::VectorType::get(intType, 4);
	}
	return intType;
}

llvm::Type *Builder::getSingleFPType() const {
	switch (mBuilderContext->mContext.mFloatingPointPrecision) {
		case Context::Double:
			return llvm::Type::getDoubleTy(module()->getContext());
		default:
			return llvm::Type::getFloatTy(module()->getContext());
	}
}

llvm::Type *Builder::getSingleIntType() const {
	return llvm::Type::getInt32Ty(module()->getContext());
}

llvm::Value *Builder::gradientNoise3D(llvm::IRBuilder<> *irBuilder, llvm::Value *fx, llvm::Value *fy, llvm::Value *fz, llvm::Value *ix, llvm::Value *iy, llvm::Value *iz, llvm::Value *seed) {
	std::vector<llvm::Value*> params;
	params.push_back(fx);
	params.push_back(fy);
	params.push_back(fz);
	params.push_back(ix);
	params.push_back(iy);
	params.push_back(iz);
	params.push_back(seed);
	return irBuilder->CreateCall(mBuilderContext->mGradientNoise3D, params);
}

llvm::Value *Builder::gradientCoherentNoise3D(llvm::IRBuilder<> *irBuilder, llvm::Value *x, llvm::Value *y, llvm::Value *z, llvm::Value *seed) {
	std::vector<llvm::Value*> params;
	params.push_back(x);
	params.push_back(y);
	params.push_back(z);
	params.push_back(seed);
	return irBuilder->CreateCall(mBuilderContext->mGradientCoherentNoise3D, params);
}

llvm::Value *Builder::valueNoise3D(llvm::Value *x, llvm::Value *y, llvm::Value *z, llvm::Value *seed) {
	return mIRBuilder->CreateCall4(mBuilderContext->mValueNoise3D, x, y, z, seed);
}

llvm::Value *Builder::interpolateCubic(llvm::IRBuilder<> *irBuilder, llvm::Value *n0, llvm::Value *n1, llvm::Value *n2, llvm::Value *n3, llvm::Value *a) {
	llvm::Value *p = irBuilder->CreateFSub(irBuilder->CreateFSub(n3, n2), irBuilder->CreateFSub(n0, n1));
	llvm::Value *q = irBuilder->CreateFSub(irBuilder->CreateFSub(n0, n1), p);
	llvm::Value *r = irBuilder->CreateFSub(n2, n0);
	llvm::Value *s = n1;

	llvm::Value *p1 = irBuilder->CreateFMul(p, irBuilder->CreateFMul(a, irBuilder->CreateFMul(a, a)));
	llvm::Value *p2 = irBuilder->CreateFMul(q, irBuilder->CreateFMul(a, a));
	llvm::Value *p3 = irBuilder->CreateFMul(r, a);
	return irBuilder->CreateFAdd(p1, irBuilder->CreateFAdd(p2, irBuilder->CreateFAdd(p3, s)));
	/*
	double p = (n3 - n2) - (n0 - n1);
	  double q = (n0 - n1) - p;
	  double r = n2 - n0;
	  double s = n1;
	  return p * a * a * a + q * a * a + r * a + s;
	*/
}

llvm::Value *Builder::interpolateLinear(llvm::IRBuilder<> *irBuilder, llvm::Value *n0, llvm::Value *n1, llvm::Value *a) {
	if (n0->getType()->isVectorTy()) {
		return irBuilder->CreateFAdd(irBuilder->CreateFMul(irBuilder->CreateFSub(getFPValue(1.0), a), n0), irBuilder->CreateFMul(a, n1));
	}
	else {
		return irBuilder->CreateFAdd(irBuilder->CreateFMul(irBuilder->CreateFSub(getSingleFPValue(1.0), a), n0), irBuilder->CreateFMul(a, n1));
	}

}

llvm::Value *Builder::sCurve3(llvm::IRBuilder<> *irBuilder, llvm::Value *a) {
	if (a->getType()->isVectorTy()) {
		return irBuilder->CreateFMul(irBuilder->CreateFMul(a, a), irBuilder->CreateFSub(getFPValue(3.0), irBuilder->CreateFMul(getFPValue(2.0), a)));
	}
	else {
		return irBuilder->CreateFMul(irBuilder->CreateFMul(a, a), irBuilder->CreateFSub(getSingleFPValue(3.0), irBuilder->CreateFMul(getSingleFPValue(2.0), a)));
	}

}

llvm::Value *Builder::sCurve5(llvm::IRBuilder<> *irBuilder, llvm::Value *a) {
	llvm::Value *a3 = irBuilder->CreateFMul(a, irBuilder->CreateFMul(a, a));
	llvm::Value *a4 = irBuilder->CreateFMul(a3, a);
	llvm::Value *a5 = irBuilder->CreateFMul(a4, a);

	if (a->getType()->isVectorTy()) {
		return irBuilder->CreateFAdd(irBuilder->CreateFSub(irBuilder->CreateFMul(getFPValue(6.0), a5), irBuilder->CreateFMul(getFPValue(15.0), a4)), irBuilder->CreateFMul(getFPValue(10.0), a3));
	}
	else {
		return irBuilder->CreateFAdd(irBuilder->CreateFSub(irBuilder->CreateFMul(getSingleFPValue(6.0), a5), irBuilder->CreateFMul(getSingleFPValue(15.0), a4)), irBuilder->CreateFMul(getSingleFPValue(10.0), a3));
	}
}

void Builder::debugPrintDouble(llvm::IRBuilder<> *irBuilder, int id, llvm::Value *d) {
	//llvm::CallInst *callInst = irBuilder->CreateCall2(mDebugPrintDouble, d, getIntValue(id));
	//callInst->setCallingConv(llvm::CallingConv::C);
}

llvm::Value *Builder::makeInt32Range(llvm::IRBuilder<> *irBuilder, llvm::Value *n) const {
	return irBuilder->CreateCall(mBuilderContext->mMakeInt32Range, n);
}

llvm::Value *Builder::vectorizeSingleValue(llvm::IRBuilder<> *irBuilder, llvm::Value *value) {
	llvm::Value *ret = llvm::UndefValue::get(llvm::VectorType::get(value->getType(), 4));
	ret = irBuilder->CreateInsertElement(ret, value, irBuilder->getInt32(0));
	ret = irBuilder->CreateInsertElement(ret, value, irBuilder->getInt32(1));
	ret = irBuilder->CreateInsertElement(ret, value, irBuilder->getInt32(2));
	ret = irBuilder->CreateInsertElement(ret, value, irBuilder->getInt32(3));
	return ret;
}


llvm::Constant *Builder::getSingleIntValue(int value) const {
	return llvm::ConstantInt::get(getSingleIntType(), (int64_t)value, true);
}

llvm::Constant *Builder::getSingleFPValue(double value) const {
	return llvm::ConstantFP::get(getSingleFPType(), value);
}

llvm::Value *Builder::fmul(llvm::Value *a, llvm::Value *b) {
	return mIRBuilder->CreateFMul(a, b);
}

llvm::Value *Builder::fadd(llvm::Value *a, llvm::Value *b) {
	return mIRBuilder->CreateFAdd(a, b);
}

llvm::Value *Builder::fsub(llvm::Value *a, llvm::Value *b) {
	return mIRBuilder->CreateFSub(a, b);
}

llvm::Value *Builder::fdiv(llvm::Value *a, llvm::Value *b) {
	return mIRBuilder->CreateFDiv(a, b);
}

llvm::Value *Builder::fpow(llvm::Value *v, llvm::Value *p) {
	if (v->getType()->isVectorTy()) {
		llvm::Value *result = llvm::UndefValue::get(getFPType());
		for (int i = 0; i < 4; i++) {
			llvm::Value *vv = mIRBuilder->CreateExtractElement(v, mIRBuilder->getInt32(i));
			llvm::Value *pp = mIRBuilder->CreateExtractElement(v, mIRBuilder->getInt32(i));
			result = mIRBuilder->CreateInsertElement(result, fpow(vv, pp), mIRBuilder->getInt32(i));
		}
		return result;
	}
	else {
		return mIRBuilder->CreateCall2(mBuilderContext->mFPPow, v, p);
	}
}

llvm::Value *Builder::fsqrt(llvm::Value *v) {
	if (v->getType()->isVectorTy()) {
		//TODO: Doesn't work on NVVM
		return mIRBuilder->CreateCall(mBuilderContext->mFPVectorSqrt, v);
	}
	else {
		return mIRBuilder->CreateCall(mBuilderContext->mFPSqrt, v);
	}

}

llvm::Value *Builder::fabs(llvm::Value *a) {
	if (mBuilderContext->mContext.mVectorize) {
		llvm::Value *result = llvm::UndefValue::get(getFPType());
		for (int i = 0; i < 4; i++) {
			llvm::Value *v = mIRBuilder->CreateExtractElement(a, mIRBuilder->getInt32(i));
			result = mIRBuilder->CreateInsertElement(result, mIRBuilder->CreateSelect(mIRBuilder->CreateFCmpOLT(v, llvm::ConstantFP::get(getSingleFPType(), 0)), mIRBuilder->CreateFNeg(v), v), mIRBuilder->getInt32(i));
		}
		return result;
	}
	else {
		return mIRBuilder->CreateSelect(mIRBuilder->CreateFCmpOLT(a, getFPValue(0)), mIRBuilder->CreateFNeg(a), a);
	}
}

llvm::Value *Builder::extractElement(llvm::Value *vec, int i) {
	return mIRBuilder->CreateExtractElement(vec, getSingleIntValue(i));
}

llvm::Value *Builder::insertElement(llvm::Value *vec, llvm::Value *newVal, int i) {
	return mIRBuilder->CreateInsertElement(vec, newVal, getSingleIntValue(i));

}

llvm::BasicBlock *Builder::createBasicBlock(const llvm::Twine &bbName, llvm::BasicBlock *insertBefore) {
	return llvm::BasicBlock::Create( module()->getContext(), bbName, mFunction, insertBefore);
}

void Builder::runFunctionPassManager(llvm::Function *func) {
	//mFPM->run(*func);
}

void Builder::setGenerationType(llvm::StructType *generationDataType) {
	mBuilderContext->mGenerationDataType = generationDataType;
}

llvm::Value *Builder::createModuleFunctionCall(llvm::Function *func, llvm::Value *x, llvm::Value *y, llvm::Value *z, llvm::Value *generationData) {
	std::vector<llvm::Value*> params;
	params.push_back(x);
	params.push_back(y);
	params.push_back(z);
	if (mBuilderContext->mGenerationDataType) params.push_back(generationData);
	return mIRBuilder->CreateCall(func, params);
}

Builder *Builder::createModuleFunction(const llvm::Twine &name, llvm::Value **xParam, llvm::Value **yParam, llvm::Value **zParam, llvm::Value **generationData, llvm::BasicBlock **basicBlock) {
	std::vector<llvm::Type*> params;
	params.push_back(getFPType()); //x
	params.push_back(getFPType()); //y
	params.push_back(getFPType()); //z
	if (mBuilderContext->mGenerationDataType) params.push_back(mBuilderContext->mGenerationDataType->getPointerTo());
	llvm::FunctionType *funcTy = llvm::FunctionType::get(getFPType(), params, false);
	llvm::Function *func = llvm::Function::Create(funcTy, llvm::Function::PrivateLinkage, name, module());
	llvm::Function::arg_iterator argI = func->arg_begin();
	*xParam = argI; argI++;
	(*xParam)->setName("x");
	*yParam = argI; argI++;
	(*yParam)->setName("y");
	*zParam = argI; argI++;
	(*zParam)->setName("z");
	if (mBuilderContext->mGenerationDataType) {
		*generationData = argI;
		(*generationData)->setName("generationData");
	}
	llvm::BasicBlock *bb = llvm::BasicBlock::Create(module()->getContext(), "basic block", func);
	*basicBlock = bb;
	Builder *builder = new Builder(mBuilderContext);
	builder->begin(func, bb);
	return builder;
}

llvm::Value *Builder::load(llvm::Value *var) {
	return mIRBuilder->CreateLoad(var);
}

void Builder::store(llvm::Value *value, llvm::Value *var) {
	mIRBuilder->CreateStore(value, var);
}

llvm::Value *Builder::clampInt(llvm::Value *val, llvm::Value *low, llvm::Value *high) {
	return mIRBuilder->CreateSelect(mIRBuilder->CreateICmpSLT(val, low), low, mIRBuilder->CreateSelect(mIRBuilder->CreateICmpSGT(val, high), high, val));
}

llvm::Value *Builder::clampFP(llvm::Value *val, llvm::Value *low, llvm::Value *high) {
	if (val->getType()->isVectorTy()) {
		llvm::Value *ret = llvm::UndefValue::get(getFPType());
		for (int i = 0; i < 4; i++) {
			llvm::Value *v = mIRBuilder->CreateExtractElement(val, getSingleIntValue(i));
			llvm::Value *l = mIRBuilder->CreateExtractElement(low, getSingleIntValue(i));
			llvm::Value *h = mIRBuilder->CreateExtractElement(high, getSingleIntValue(i));
			mIRBuilder->CreateInsertElement(ret, clampFP(v, l, h), getSingleIntValue(i));
		}
		return ret;
	}
	else {
		return mIRBuilder->CreateSelect(mIRBuilder->CreateFCmpOGT(val, high), high, mIRBuilder->CreateSelect(mIRBuilder->CreateFCmpOLT(val, low), low, val));
	}
}

llvm::Value *Builder::minFP(llvm::Value *a, llvm::Value *b) {
	if (a->getType()->isVectorTy()) {
		llvm::Value *ret = llvm::UndefValue::get(getFPType());
		for (int i = 0; i < 4; i++) {
			llvm::Value *av = mIRBuilder->CreateExtractElement(a, getSingleIntValue(i));
			llvm::Value *bv = mIRBuilder->CreateExtractElement(b, getSingleIntValue(i));
			mIRBuilder->CreateInsertElement(ret, minFP(av, bv), getSingleIntValue(i));
		}
		return ret;
	}
	else {
		return mIRBuilder->CreateSelect(mIRBuilder->CreateFCmpOLT(a, b), a, b);
	}
}

llvm::Value *Builder::maxFP(llvm::Value *a, llvm::Value *b) {
	if (a->getType()->isVectorTy()) {
		llvm::Value *ret = llvm::UndefValue::get(getFPType());
		for (int i = 0; i < 4; i++) {
			llvm::Value *av = mIRBuilder->CreateExtractElement(a, getSingleIntValue(i));
			llvm::Value *bv = mIRBuilder->CreateExtractElement(b, getSingleIntValue(i));
			mIRBuilder->CreateInsertElement(ret, maxFP(av, bv), getSingleIntValue(i));
		}
		return ret;
	}
	else {
		return mIRBuilder->CreateSelect(mIRBuilder->CreateFCmpOGT(a, b), a, b);
	}
}

llvm::Value *Builder::i1x4toi1(llvm::Value *v) {
	if (!v->getType()->isVectorTy()) return v;

	llvm::Value *v1 = mIRBuilder->CreateExtractElement(v, mIRBuilder->getInt32(0));
	llvm::Value *v2 = mIRBuilder->CreateExtractElement(v, mIRBuilder->getInt32(1));
	llvm::Value *v3 = mIRBuilder->CreateExtractElement(v, mIRBuilder->getInt32(2));
	llvm::Value *v4 = mIRBuilder->CreateExtractElement(v, mIRBuilder->getInt32(3));
	return mIRBuilder->CreateAdd(mIRBuilder->CreateAnd(v1, v2), mIRBuilder->CreateAnd(v3, v4));
}

void Builder::setInsertPoint(llvm::BasicBlock *bb) {
	mIRBuilder->SetInsertPoint(bb);
}

void Builder::branch(llvm::BasicBlock *bb) {
	mIRBuilder->CreateBr(bb);
}

void Builder::conditionalBranch(llvm::Value *cond, llvm::BasicBlock *ifTrue, llvm::BasicBlock *ifFalse) {
	mIRBuilder->CreateCondBr(cond, ifTrue, ifFalse);
}


void Builder::generateGradientNoise3DFunction(llvm::Module *module){
	std::vector<llvm::Type*> params;
	params.push_back(getFPType()); //fx
	params.push_back(getFPType()); //fy
	params.push_back(getFPType()); //fz
	params.push_back(getIntType()); //ix
	params.push_back(getIntType()); //iy
	params.push_back(getIntType()); //iz
	params.push_back(getIntType()); //seed
	llvm::FunctionType *funcTy = llvm::FunctionType::get(getFPType(), params, false);
	mBuilderContext->mGradientNoise3D = llvm::Function::Create(funcTy, llvm::Function::PrivateLinkage, "gradientNoise3D", module);
	llvm::BasicBlock *bb = llvm::BasicBlock::Create(module->getContext(), "basic block", mBuilderContext->mGradientNoise3D);
	llvm::IRBuilder<> irBuilder(bb);

	llvm::Function::arg_iterator argI = mBuilderContext->mGradientNoise3D->arg_begin();
	llvm::Value *fx = argI; argI++;
	fx->setName("fx");
	llvm::Value *fy = argI; argI++;
	fy->setName("fy");
	llvm::Value *fz = argI; argI++;
	fz->setName("fz");
	llvm::Value *ix = argI; argI++;
	ix->setName("ix");
	llvm::Value *iy = argI; argI++;
	iy->setName("iy");
	llvm::Value *iz = argI; argI++;
	iz->setName("iz");
	llvm::Value *seed = argI;
	seed->setName("seed");

	llvm::Value *xNoiseGen = getIntValue(X_NOISE_GEN);
	llvm::Value *yNoiseGen = getIntValue(Y_NOISE_GEN);
	llvm::Value *zNoiseGen = getIntValue(Z_NOISE_GEN);
	llvm::Value *seedNoiseGen = getIntValue(SEED_NOISE_GEN);
	llvm::Value *xx = irBuilder.CreateMul(xNoiseGen, ix); //X_NOISE_GEN    * ix
	llvm::Value *yy = irBuilder.CreateMul(yNoiseGen, iy);
	llvm::Value *zz = irBuilder.CreateMul(zNoiseGen, iz);
	llvm::Value *ss = irBuilder.CreateMul(seedNoiseGen, seed);
	llvm::Value *nn = irBuilder.CreateAdd(irBuilder.CreateAdd(irBuilder.CreateAdd(xx, yy), zz), ss);  //xx + yy + zz + ss
	llvm::Value *vectorIndex = irBuilder.CreateAnd(nn, getIntValue(0x7fffffff)); //vectorIndex = nn & 0x7fffffff

	vectorIndex = irBuilder.CreateXor(vectorIndex, irBuilder.CreateLShr(vectorIndex, getIntValue(SHIFT_NOISE_GEN))); //vectorIndex ^= (vectorIndex >> SHIFT_NOISE_GEN);
	vectorIndex = irBuilder.CreateAnd(vectorIndex, getIntValue(0xff)); //vectorIndex &= 0xff;
	vectorIndex = irBuilder.CreateShl(vectorIndex, getIntValue(2)); //vectorIndex = vectorIndex << 2;

	llvm::Value *xvIndex = vectorIndex;
	llvm::Value *yvIndex = irBuilder.CreateAdd(vectorIndex, getIntValue(1));
	llvm::Value *zvIndex = irBuilder.CreateAdd(vectorIndex, getIntValue(2));

	llvm::Value *xvGradient;
	llvm::Value *yvGradient;
	llvm::Value *zvGradient;
	if (mBuilderContext->mContext.mVectorize) {
		xvGradient = llvm::UndefValue::get(getFPType());
		yvGradient = llvm::UndefValue::get(getFPType());
		zvGradient = llvm::UndefValue::get(getFPType());
		for (int i = 0; i < 4; ++i) {
			std::vector<llvm::Value*> gepParam;
			gepParam.push_back(irBuilder.getInt32(0));
			gepParam.push_back(irBuilder.CreateExtractElement(xvIndex, irBuilder.getInt32(i)));
			llvm::Value *x = irBuilder.CreateLoad(irBuilder.CreateGEP(mBuilderContext->mRandomVectors, gepParam));
			gepParam[1] = irBuilder.CreateExtractElement(yvIndex, irBuilder.getInt32(i));
			llvm::Value *y = irBuilder.CreateLoad(irBuilder.CreateGEP(mBuilderContext->mRandomVectors, gepParam));
			gepParam[1] = irBuilder.CreateExtractElement(zvIndex, irBuilder.getInt32(i));
			llvm::Value *z = irBuilder.CreateLoad(irBuilder.CreateGEP(mBuilderContext->mRandomVectors, gepParam));
			xvGradient = irBuilder.CreateInsertElement(xvGradient, x, irBuilder.getInt32(i));
			yvGradient = irBuilder.CreateInsertElement(yvGradient, y, irBuilder.getInt32(i));
			zvGradient = irBuilder.CreateInsertElement(zvGradient, z, irBuilder.getInt32(i));
		}

	}
	else {
		std::vector<llvm::Value*> gepParam;
		gepParam.push_back(irBuilder.getInt32(0));
		gepParam.push_back(xvIndex);
		xvGradient = irBuilder.CreateLoad(irBuilder.CreateGEP(mBuilderContext->mRandomVectors, gepParam));
		gepParam[1] = yvIndex;
		yvGradient = irBuilder.CreateLoad(irBuilder.CreateGEP(mBuilderContext->mRandomVectors, gepParam));
		gepParam[1] = zvIndex;
		zvGradient = irBuilder.CreateLoad(irBuilder.CreateGEP(mBuilderContext->mRandomVectors, gepParam));
	}

	llvm::Value *xvPoint = irBuilder.CreateFSub(fx, irBuilder.CreateSIToFP(ix, getFPType()));
	llvm::Value *yvPoint = irBuilder.CreateFSub(fy, irBuilder.CreateSIToFP(iy, getFPType()));
	llvm::Value *zvPoint = irBuilder.CreateFSub(fz, irBuilder.CreateSIToFP(iz, getFPType()));

	llvm::Value *x = irBuilder.CreateFMul(xvGradient, xvPoint);
	llvm::Value *y = irBuilder.CreateFMul(yvGradient, yvPoint);
	llvm::Value *z = irBuilder.CreateFMul(zvGradient, zvPoint);

	llvm::Value *ret = irBuilder.CreateFMul(irBuilder.CreateFAdd(x, irBuilder.CreateFAdd(y, z)), getFPValue(2.12));
	irBuilder.CreateRet(ret);

	runFunctionPassManager(mBuilderContext->mGradientNoise3D);
}

void Builder::generateGradientCoherentNoise3DFunction(llvm::Module *module) {

	//llvm::IRBuilder *irBuilder, llvm::Value *x, llvm::Value *y, llvm::Value *z, llvm::Value *seed
	std::vector<llvm::Type*> params;
	params.push_back(getFPType()); //x
	params.push_back(getFPType()); //y
	params.push_back(getFPType()); //z
	params.push_back(getIntType()); //seed
	llvm::FunctionType *funcTy = llvm::FunctionType::get(getFPType(), params, false);
	mBuilderContext->mGradientCoherentNoise3D = llvm::Function::Create(funcTy, llvm::Function::PrivateLinkage, "gradientCoherentNoise3D", module);
	llvm::BasicBlock *bb = llvm::BasicBlock::Create(module->getContext(), "basic block", mBuilderContext->mGradientCoherentNoise3D);
	llvm::IRBuilder<> irBuilder(bb);

	llvm::Function::arg_iterator argI = mBuilderContext->mGradientCoherentNoise3D->arg_begin();
	llvm::Value *x = argI; argI++;
	x->setName("x");
	llvm::Value *y = argI; argI++;
	y->setName("y");
	llvm::Value *z = argI; argI++;
	z->setName("z");
	llvm::Value *seed = argI;
	seed->setName("seed");

	debugPrintDouble(&irBuilder, 20, x);
	debugPrintDouble(&irBuilder, 21, y);
	debugPrintDouble(&irBuilder, 22, z);

	llvm::Value *xi = irBuilder.CreateFPToSI(x, getIntType());
	llvm::Value *yi = irBuilder.CreateFPToSI(y, getIntType());
	llvm::Value *zi = irBuilder.CreateFPToSI(z, getIntType());

	llvm::Value *x0 = irBuilder.CreateSelect(irBuilder.CreateFCmpUGT(x, getFPValue(0.0)), xi, irBuilder.CreateSub(xi, getIntValue(1)));
	llvm::Value *x1 = irBuilder.CreateAdd(x0, getIntValue(1));

	llvm::Value *y0 = irBuilder.CreateSelect(irBuilder.CreateFCmpUGT(y, getFPValue(0.0)), yi, irBuilder.CreateSub(yi, getIntValue(1)));
	llvm::Value *y1 = irBuilder.CreateAdd(y0, getIntValue(1));

	llvm::Value *z0 = irBuilder.CreateSelect(irBuilder.CreateFCmpUGT(z, getFPValue(0.0)), zi, irBuilder.CreateSub(zi, getIntValue(1)));
	llvm::Value *z1 = irBuilder.CreateAdd(z0, getIntValue(1));


	llvm::Value *xs = irBuilder.CreateFSub(x, irBuilder.CreateSIToFP(x0, getFPType()));
	llvm::Value *ys = irBuilder.CreateFSub(y, irBuilder.CreateSIToFP(y0, getFPType()));
	llvm::Value *zs = irBuilder.CreateFSub(z, irBuilder.CreateSIToFP(z0, getFPType()));
	debugPrintDouble(&irBuilder, 100, xs);
	debugPrintDouble(&irBuilder, 101, ys);
	debugPrintDouble(&irBuilder, 102, zs);
	switch (mBuilderContext->mContext.mNoiseQuality) {
		case Context::Fast:
			break;
		case Context::Standard:
			xs = sCurve3(&irBuilder, xs);
			ys = sCurve3(&irBuilder, ys);
			zs = sCurve3(&irBuilder, zs);
			break;
		case Context::Best:
			xs = sCurve5(&irBuilder, xs);
			ys = sCurve5(&irBuilder, ys);
			zs = sCurve5(&irBuilder, zs);
			break;
		default:
			assert("Invalid Context::mNoiseQuality");
	}

	debugPrintDouble(&irBuilder, 103, xs);
	debugPrintDouble(&irBuilder, 104, ys);
	debugPrintDouble(&irBuilder, 105, zs);

	llvm::Value *n0, *n1, *ix0, *ix1, *iy0, *iy1;
	n0 = gradientNoise3D(&irBuilder, x, y, z, x0, y0, z0, seed);
	n1 = gradientNoise3D(&irBuilder, x, y, z, x1, y0, z0, seed);
	debugPrintDouble(&irBuilder, 25, n0);
	debugPrintDouble(&irBuilder, 26, n1);
	ix0 = interpolateLinear(&irBuilder, n0, n1, xs);

	n0 = gradientNoise3D(&irBuilder, x, y, z, x0, y1, z0, seed);
	n1 = gradientNoise3D(&irBuilder, x, y, z, x1, y1, z0, seed);
	debugPrintDouble(&irBuilder, 27, n0);
	debugPrintDouble(&irBuilder, 28, n1);
	ix1 = interpolateLinear(&irBuilder, n0, n1, xs);
	iy0 = interpolateLinear(&irBuilder, ix0, ix1, ys);
	debugPrintDouble(&irBuilder, 35, iy0);

	n0 = gradientNoise3D(&irBuilder, x, y, z, x0, y0, z1, seed);
	n1 = gradientNoise3D(&irBuilder, x, y, z, x1, y0, z1, seed);
	debugPrintDouble(&irBuilder, 29, n0);
	debugPrintDouble(&irBuilder, 30, n1);
	ix0 = interpolateLinear(&irBuilder, n0, n1, xs);

	n0 = gradientNoise3D(&irBuilder, x, y, z, x0, y1, z1, seed);
	n1 = gradientNoise3D(&irBuilder, x, y, z, x1, y1, z1, seed);
	debugPrintDouble(&irBuilder, 31, n0);
	debugPrintDouble(&irBuilder, 32, n1);
	ix1 = interpolateLinear(&irBuilder, n0, n1, xs);
	iy1 = interpolateLinear(&irBuilder, ix0, ix1, ys);
	debugPrintDouble(&irBuilder, 36, iy1);

	llvm::Value *ret = interpolateLinear(&irBuilder, iy0, iy1, zs);
	debugPrintDouble(&irBuilder, 37, ret);
	irBuilder.CreateRet(ret);

	runFunctionPassManager(mBuilderContext->mGradientCoherentNoise3D);
}

void Builder::generateDebugPrinterFunction(llvm::Module *module) {
	std::vector<llvm::Type*> params;
	params.push_back(llvm::Type::getDoubleTy(module->getContext())); //d
	params.push_back(llvm::Type::getInt32Ty(module->getContext())); //id
	llvm::FunctionType *funcTy = llvm::FunctionType::get(llvm::Type::getVoidTy(module->getContext()), params, false);
	mBuilderContext->mDebugPrintDouble = llvm::Function::Create(funcTy, llvm::Function::ExternalLinkage, "debugPrintDouble", module);
}

void Builder::generateIntMake32RangeFunction(llvm::Module *module) {
	std::vector<llvm::Type*> params;
	params.push_back(getFPType()); //x
	llvm::FunctionType *funcTy = llvm::FunctionType::get(getFPType(), params, false);
	mBuilderContext->mMakeInt32Range = llvm::Function::Create(funcTy, llvm::Function::PrivateLinkage, "makeInt32Range", module);
	llvm::Value *n = mBuilderContext->mMakeInt32Range->arg_begin();

	if (!mBuilderContext->mContext.mVectorize) {
		llvm::BasicBlock *bb1 = llvm::BasicBlock::Create(module->getContext(), "cond1", mBuilderContext->mMakeInt32Range);
		llvm::IRBuilder<> irBuilder(bb1);
		llvm::BasicBlock *bb2 = llvm::BasicBlock::Create(module->getContext(), "cond1True", mBuilderContext->mMakeInt32Range);
		llvm::BasicBlock *bb3 = llvm::BasicBlock::Create(module->getContext(), "cond2", mBuilderContext->mMakeInt32Range);
		llvm::BasicBlock *bb4 = llvm::BasicBlock::Create(module->getContext(), "cond2True", mBuilderContext->mMakeInt32Range);
		llvm::BasicBlock *bb5 = llvm::BasicBlock::Create(module->getContext(), "else", mBuilderContext->mMakeInt32Range);
		llvm::Value *cond = irBuilder.CreateFCmpOGE(n, getFPValue(1073741824.0));
		irBuilder.CreateCondBr(cond, bb2, bb3);

		irBuilder.SetInsertPoint(bb2);
		llvm::Value *ret = irBuilder.CreateFSub(irBuilder.CreateFMul(getFPValue(2.0), irBuilder.CreateFRem(n, getFPValue(1073741824.0))), getFPValue(1073741824.0));
		irBuilder.CreateRet(ret);

		irBuilder.SetInsertPoint(bb3);
		cond = irBuilder.CreateFCmpOLE(n, getFPValue(-1073741824.0));
		irBuilder.CreateCondBr(cond, bb4, bb5);

		irBuilder.SetInsertPoint(bb4);
		ret = irBuilder.CreateFAdd(irBuilder.CreateFMul(getFPValue(2.0), irBuilder.CreateFRem(n, getFPValue(1073741824.0))), getFPValue(1073741824.0));
		irBuilder.CreateRet(ret);

		irBuilder.SetInsertPoint(bb5);
		irBuilder.CreateRet(n);

	}
	else {

		llvm::BasicBlock *basicBlocks[5 * 4 + 1];
		for (int i = 0; i < 21; i++) {
			basicBlocks[i] = llvm::BasicBlock::Create(module->getContext(), "bb1", mBuilderContext->mMakeInt32Range);
		}
		llvm::IRBuilder<> irBuilder(basicBlocks[0]);
		llvm::AllocaInst *resultVar = irBuilder.CreateAlloca(getFPType());
		llvm::Value *result = llvm::UndefValue::get(getFPType());
		irBuilder.CreateStore(result, resultVar);
		int basicBlockId = 0;
		for (int i = 0; i < 4; ++i) {
			//1
			irBuilder.SetInsertPoint(basicBlocks[basicBlockId]);
			llvm::Value *val = irBuilder.CreateExtractElement(n, irBuilder.getInt32(i));
			llvm::Value *cond = irBuilder.CreateFCmpOGE(val, llvm::ConstantFP::get(getSingleFPType(), 1073741824.0));
			irBuilder.CreateCondBr(cond, basicBlocks[basicBlockId + 1], basicBlocks[basicBlockId + 2]);

			//2
			irBuilder.SetInsertPoint(basicBlocks[++basicBlockId]);
			llvm::Value *ret = irBuilder.CreateFSub(irBuilder.CreateFMul(llvm::ConstantFP::get(getSingleFPType(), 2.0), irBuilder.CreateFRem(val, llvm::ConstantFP::get(getSingleFPType(), 1073741824.0))), llvm::ConstantFP::get(getSingleFPType(), 1073741824.0));
			result = irBuilder.CreateLoad(resultVar);
			result =  irBuilder.CreateInsertElement(result, ret, irBuilder.getInt32(i));
			irBuilder.CreateStore(result, resultVar);
			irBuilder.CreateBr(basicBlocks[basicBlockId + 4]);

			//3
			irBuilder.SetInsertPoint(basicBlocks[++basicBlockId]);
			cond = irBuilder.CreateFCmpOLE(val, llvm::ConstantFP::get(getSingleFPType(), -1073741824.0));
			irBuilder.CreateCondBr(cond, basicBlocks[basicBlockId + 1], basicBlocks[basicBlockId + 2]);

			//4
			irBuilder.SetInsertPoint(basicBlocks[++basicBlockId]);
			ret = irBuilder.CreateFAdd(irBuilder.CreateFMul(llvm::ConstantFP::get(getSingleFPType(), 2.0), irBuilder.CreateFRem(val, llvm::ConstantFP::get(getSingleFPType(), 1073741824.0))), llvm::ConstantFP::get(getSingleFPType(), 1073741824.0));
			result = irBuilder.CreateLoad(resultVar);
			irBuilder.CreateInsertElement(result, ret, irBuilder.getInt32(i));
			irBuilder.CreateStore(result, resultVar);
			irBuilder.CreateBr(basicBlocks[basicBlockId + 2]);

			//5
			irBuilder.SetInsertPoint(basicBlocks[++basicBlockId]);
			result = irBuilder.CreateLoad(resultVar);
			result = irBuilder.CreateInsertElement(result, val, irBuilder.getInt32(i));
			irBuilder.CreateStore(result, resultVar);
			irBuilder.CreateBr(basicBlocks[basicBlockId + 1]);

			++basicBlockId;
		}
		irBuilder.SetInsertPoint(basicBlocks[basicBlockId]);
		irBuilder.CreateRet(irBuilder.CreateLoad(resultVar));
	}
}

void Builder::generateValueNoise3DFunction(llvm::Module *module) {
	std::vector<llvm::Type*> params;
	params.push_back(getIntType()); //x
	params.push_back(getIntType()); //y
	params.push_back(getIntType()); //z
	params.push_back(getIntType()); //seed
	llvm::FunctionType *funcTy = llvm::FunctionType::get(getFPType(), params, false);
	mBuilderContext->mValueNoise3D = llvm::Function::Create(funcTy, llvm::Function::PrivateLinkage, "valueNoise3D", module);
	llvm::BasicBlock *bb = llvm::BasicBlock::Create(module->getContext(), "basic block", mBuilderContext->mValueNoise3D);
	llvm::IRBuilder<> irBuilder(bb);

	llvm::Function::arg_iterator argI = mBuilderContext->mValueNoise3D->arg_begin();
	llvm::Value *x = argI; argI++;
	x->setName("x");
	llvm::Value *y = argI; argI++;
	y->setName("y");
	llvm::Value *z = argI; argI++;
	y->setName("z");
	llvm::Value *seed = argI;
	seed->setName("seed");

	llvm::Value *xNoiseGen = getIntValue(X_NOISE_GEN);
	llvm::Value *yNoiseGen = getIntValue(Y_NOISE_GEN);
	llvm::Value *zNoiseGen = getIntValue(Z_NOISE_GEN);
	llvm::Value *seedNoiseGen = getIntValue(SEED_NOISE_GEN);
	llvm::Value *xx = irBuilder.CreateMul(xNoiseGen, x); //X_NOISE_GEN    * x
	llvm::Value *yy = irBuilder.CreateMul(yNoiseGen, y);
	llvm::Value *zz = irBuilder.CreateMul(zNoiseGen, z);
	llvm::Value *ss = irBuilder.CreateMul(seedNoiseGen, seed);
	llvm::Value *nn = irBuilder.CreateAdd(irBuilder.CreateAdd(irBuilder.CreateAdd(xx, yy), zz), ss);  //xx + yy + zz + ss
	llvm::Value *n = irBuilder.CreateAnd(nn, getIntValue(0x7fffffff)); //n = nn & 0x7fffffff
	n = irBuilder.CreateXor(irBuilder.CreateLShr(n, getIntValue(13)), n); //n = (n >> 13) ^ n;
	llvm::Value *s1 = irBuilder.CreateAdd(irBuilder.CreateMul(irBuilder.CreateMul(n, n), getIntValue(60493)), getIntValue(19990303)); //s1 = (n * n * 60493 + 19990303)
	llvm::Value *s2 = irBuilder.CreateAdd(irBuilder.CreateMul(n, s1), getIntValue(1376312589)); //s2 = n * s1 + 1376312589
	llvm::Value *intValueNoise = irBuilder.CreateAnd(s2, getIntValue(0x7fffffff)); // s2 & 0x7fffffff

	llvm::Value *fv = irBuilder.CreateSIToFP(intValueNoise, getFPType());
	llvm::Value *ret = irBuilder.CreateFSub(getFPValue(1.0), irBuilder.CreateFDiv(fv,  getFPValue(1073741824.0)));
	irBuilder.CreateRet(ret);
}

void Builder::defineStandardFunctions(llvm::Module *module) {
	Context::FloatingPointPrecision fpPrec = mBuilderContext->mContext.mFloatingPointPrecision;
	std::vector<llvm::Type*> powParams;
	powParams.push_back(getSingleFPType());
	powParams.push_back(getSingleFPType());
	if (fpPrec == Context::Single) {
		mBuilderContext->mFPPow = llvm::Function::Create(llvm::FunctionType::get(getSingleFPType(), powParams, false), llvm::Function::ExternalLinkage, "llvm.pow.f32", module);
		mBuilderContext->mFPSqrt = llvm::Function::Create(llvm::FunctionType::get(getSingleFPType(), std::vector<llvm::Type*>(1, getSingleFPType()), false), llvm::Function::ExternalLinkage, "llvm.sqrt.f32", module);
		if (context().mVectorize) mBuilderContext->mFPVectorSqrt = llvm::Function::Create(llvm::FunctionType::get(getFPType(), std::vector<llvm::Type*>(1, getFPType()), false), llvm::Function::ExternalLinkage, "llvm.sqrt.v4f32", module);
	}
	else {
		mBuilderContext->mFPPow = llvm::Function::Create(llvm::FunctionType::get(getSingleFPType(), powParams, false), llvm::Function::ExternalLinkage, "llvm.pow.f64", module);
		mBuilderContext->mFPSqrt = llvm::Function::Create(llvm::FunctionType::get(getSingleFPType(), std::vector<llvm::Type*>(1, getSingleFPType()), false), llvm::Function::ExternalLinkage, "llvm.sqrt.f64", module);
		if (context().mVectorize) mBuilderContext->mFPVectorSqrt = llvm::Function::Create(llvm::FunctionType::get(getFPType(), std::vector<llvm::Type*>(1, getFPType()), false), llvm::Function::ExternalLinkage, "llvm.sqrt.v4f64", module);
	}


}

Builder::BuilderContext::BuilderContext(const Context &context) :
	mContext(context),
	mDebugPrintDouble(0),
	mFPM(0),
	mMakeInt32Range(0),
	mModule(0),
	mGenerationDataType(0),
	mFPPow(0),
	mFPSqrt(0),
	mFPVectorSqrt(0)
{
}

/*
	llvm::Value *xvGradient;
	llvm::Value *yvGradient;
	llvm::Value *zvGradient;
	if (mBuilderContext->mContext.mVectorize) {
		xvGradient = llvm::UndefValue::get(getFPType());
		yvGradient = llvm::UndefValue::get(getFPType());
		zvGradient = llvm::UndefValue::get(getFPType());
		for (int i = 0; i < 4; ++i) {
			std::vector<llvm::Value*> gepParam;
			gepParam.push_back(irBuilder.getInt32(0));
			gepParam.push_back(irBuilder.CreateExtractElement(xvIndex, irBuilder.getInt32(i)));
			llvm::Value *x = irBuilder.CreateLoad(irBuilder.CreateGEP(mRandomVectors, gepParam));
			gepParam[1] = irBuilder.CreateExtractElement(yvIndex, irBuilder.getInt32(i));
			llvm::Value *y = irBuilder.CreateLoad(irBuilder.CreateGEP(mRandomVectors, gepParam));
			gepParam[1] = irBuilder.CreateExtractElement(zvIndex, irBuilder.getInt32(i));
			llvm::Value *z = irBuilder.CreateLoad(irBuilder.CreateGEP(mRandomVectors, gepParam));
			irBuilder.CreateInsertElement(xvGradient, x, irBuilder.getInt32(i));
			irBuilder.CreateInsertElement(yvGradient, y, irBuilder.getInt32(i));
			irBuilder.CreateInsertElement(zvGradient, z, irBuilder.getInt32(i));
		}

	}
	else {
		std::vector<llvm::Value*> gepParam;
		gepParam.push_back(irBuilder.getInt32(0));
		gepParam.push_back(xvIndex);
		xvGradient = irBuilder.CreateLoad(irBuilder.CreateGEP(mRandomVectors, gepParam));
		gepParam[1] = yvIndex;
		yvGradient = irBuilder.CreateLoad(irBuilder.CreateGEP(mRandomVectors, gepParam));
		gepParam[1] = zvIndex;
		zvGradient = irBuilder.CreateLoad(irBuilder.CreateGEP(mRandomVectors, gepParam));
	}
*/




}
