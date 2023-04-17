//
//  myst_fixes.h
//  scummvm
//
//  Created by Python Blue on 7/7/21.
//

#ifndef MOHAWK_MYST_FIXES_H
#define MOHAWK_MYST_FIXES_H

#include "mohawk/myst.h"

namespace Mohawk {
class MohawkEngine_Myst;

class MystFixes
{
public:
    MystFixes(MohawkEngine_Myst *vm)
    {
    }
    TransitionType TransitionFix(TransitionType transition, MystStack stack, uint16 inCard, uint16 outCard, Common::Point position)
{
    switch (stack)
    {
        case kMystStack:
        {
            if (outCard != 0)
            {
                switch (inCard)
                {
                    case 4006:
                        if (outCard == 4003)
                            return kTransitionScrollToBottom;
                        break;
                    case 4103:
                    {
                        if (outCard == 4097 && position.x < 272)
                            return kTransitionScrollToRight;
                        else if (outCard == 4097)
                            return kTransitionScrollToLeft;
                        else if (outCard == 4101 || outCard == 4100)
                            return kTransitionRightToLeft;
                        break;
                    }
                    case 4138:
                    {
                        if (outCard == 4134 || outCard == 4137)
                            return kTransitionScrollToLeft;
                        if (outCard == 4142)
                            return kTransitionScrollToRight;
                        return kTransitionDissolve;
                        break;
                    }
                    case 4143:
                    {
                        if ((outCard == 4149 || outCard == 4148) && position.x < 172)
                            return kTransitionScrollToRight;
                        break;
                    }
                    case 4484:
                    case 4396:
                    case 4615:
                        return kTransitionScrollToTop;
                        break;
                    case 4489:
                    {
                        if (outCard == 4490)
                            return kTransitionCopy;
                        if (outCard == 4491 && position.x < 272)
                            return kTransitionScrollToRight;
                        else if (outCard == 4491)
                            return kTransitionScrollToLeft;
                        break;
                    }
                    case 4490:
                    {
                        if (outCard == 4489)
                            return kTransitionCopy;
                        if (outCard == 4492 && position.x < 272)
                            return kTransitionScrollToRight;
                        else if (outCard == 4492)
                            return kTransitionScrollToLeft;
                        break;
                    }
                    case 4033:
                    case 4034:
                    case 4035:
                    case 4036:
                    case 4037:
                    case 4038:
                    case 4039:
                    case 4040:
                    case 4041:
                    case 4042:
                    case 4043:
                    case 4055:
                    case 4058:
                    case 4059:
                    case 4066:
                    case 4078:
                    case 4091:
                    case 4289:
                    case 4290:
                    case 4366:
                    case 4373:
                    case 4377:
                    case 4417:
                    case 4424:
                    case 4648:
                    case 4649:
                    case 4716:
                    case 4725:
                    case 4727:
                    case 5060:
                    case 5080:
                    case 6068:
                        return kTransitionDissolve;
                        break;
                    case 4240:
                    case 4242:
                    case 4245:
                    case 4246:
                    case 4632:
                        return kTransitionScrollToBottom;
                        break;
                    case 4379:
                        if (outCard == 4383)
                            return kTransitionRightToLeft;
                        if (outCard == 4451)
                            return kTransitionCopy;
                        break;
                    case 4393:
                    case 4394:
                        if (outCard == 4395 && position.x < 272)
                            return kTransitionScrollToRight;
                        if (outCard == 4395)
                            return kTransitionScrollToLeft;
                        break;
                    case 4451:
                        if (outCard == 4452)
                            return kTransitionRightToLeft;
                        break;
                    case 4452:
                        if (outCard == 4451)
                            return kTransitionLeftToRight;
                        break;
                    case 4430:
                        if (outCard == 4431)
                            return kTransitionScrollToBottom;
                        break;
                    case 4431:
                        if (outCard == 4430)
                            return kTransitionScrollToTop;
                        break;
                    case 4441:
                        if (outCard == 4442)
                            return kTransitionScrollToBottom;
                        break;
                    case 4442:
                        if (outCard == 4441)
                            return kTransitionScrollToTop;
                        break;
                    case 4455:
                        if (outCard == 4457)
                            return kTransitionScrollToTop;
                        break;
                    case 4457:
                        if (outCard == 4455)
                            return kTransitionScrollToBottom;
                        break;
                    case 4466:
                        if (outCard == 4467)
                            return kTransitionScrollToTop;
                        break;
                    case 4467:
                        if (outCard == 4466)
                            return kTransitionScrollToBottom;
                        break;
                    default:
                        break;
                }
            }
            else
            {
                switch (inCard)
                {
                    default:
                        break;
                }
            }
            switch (outCard)
            {
                case 4006:
                case 4240:
                case 4242:
                case 4245:
                case 4246:
                case 4632:
                    return kTransitionScrollToTop;
                case 4159:
                case 4160:
                {
                    if (inCard == 4374)
                        return kTransitionScrollToLeft;
                    if (inCard == 4162)
                        return kTransitionPushToTop;
                }
                case 4165:
                case 4166:
                {
                    if (inCard == 4162)
                        return kTransitionPushToTop;
                    else
                        return kTransitionDissolve;
                }
                case 4323:
                {
                    if ((inCard == 4317 || inCard == 4319) && position.x < 272)
                        return kTransitionScrollToRight;
                    else if (inCard == 4317 || inCard == 4319)
                        return kTransitionScrollToLeft;
                    else
                        return kTransitionDissolve;
                }
                case 4383:
                    if (inCard == 4394)
                        return kTransitionScrollToLeft;
                case 4451:
                    if (inCard == 4427)
                        return kTransitionScrollToLeft;
                case 4630:
                    return kTransitionScrollToLeft;
                case 4484:
                case 4396:
                case 4615:
                    return kTransitionScrollToBottom;
                case 4734:
                {
                    if (inCard == 4730 && position.x < 272)
                        return kTransitionScrollToRight;
                    else if (inCard == 4730)
                        return kTransitionScrollToLeft;
                    else
                        return kTransitionDissolve;
                }
                case 4552:
                    if (inCard == 4537)
                        return kTransitionScrollToRight;
                    if (inCard == 4550)
		    {
			if (position.x < 272)
			{
                        	return kTransitionScrollToRight;
			}
			else
			{
                        	return kTransitionScrollToLeft;
			}
		    }
                case 4033:
                case 4034:
                case 4035:
                case 4036:
                case 4037:
                case 4038:
                case 4039:
                case 4040:
                case 4041:
                case 4042:
                case 4043:
                case 4055:
                case 4058:
                case 4059:
                case 4066:
                case 4078:
                case 4091:
                case 4289:
                case 4290:
                case 4377:
                case 4648:
                case 4649:
                case 4716:
                case 4725:
                case 4727:
                case 5060:
                case 5080:
                case 6068:
                    return kTransitionDissolve;
                case 4443:
                    if (inCard == 4467)
                        return kTransitionDissolve;
                    break;
                case 4452:
                    if (inCard == 4427)
                        return kTransitionScrollToLeft;
                    break;
                default:
                    break;
            }
        }
        case kSeleniticStack:
        {
            if (outCard != 0)
            {
                switch (inCard)
                {
                    case 1074:
                        if (outCard == 1075)
                            return kTransitionDissolve;
                        break;
                    case 1075:
                    case 1076:
                        return kTransitionDissolve;
                        break;
                    case 1092:
                        if (outCard == 1117)
                            return kTransitionScrollToTop;
                        break;
                    case 1117:
                        if (outCard == 1092)
                            return kTransitionScrollToBottom;
                        break;
                    case 1231:
                    case 1253:
                        if (outCard == 1276 || outCard == 1279)
                            return kTransitionScrollToTop;
                        break;
                    case 1245:
                        return kTransitionDissolve;
                        break;
                    case 1258:
                    case 1259:
                    case 1272:
                        if (outCard == 1277 || outCard == 1278)
                            return kTransitionScrollToBottom;
                        break;
                    case 1276:
                    case 1279:
                        if (outCard == 1231 || outCard == 1253)
                            return kTransitionScrollToBottom;
                        else if (outCard == 1277 || outCard == 1278)
                            return kTransitionScrollToTop;
                        break;
                    case 1277:
                    case 1278:
                        if (outCard == 1276 || outCard == 1279)
                            return kTransitionScrollToBottom;
                        else if (outCard == 1258 || outCard == 1259 || outCard == 1272)
                            return kTransitionScrollToTop;
                        break;
                    default:
                        break;
                }
            }
            switch (inCard)
            {
                case 1139:
                case 1140:
                    if (transition == kTransitionBottomToTop)
                        return kTransitionPushToTop;
                    if (transition == kTransitionTopToBottom)
                        return kTransitionSlideToBottom;
                default:
                    break;
            }
        }
        case kMechanicalStack:
        {
            if (outCard != 0)
            {
                if (outCard == 6154 && (inCard == 6156 || inCard == 6339))
                    return kNoTransition;
                else if (outCard == 6151 && inCard != 6155)
                    return kNoTransition;
                switch (inCard)
                {
                    case 6150:
                    case 6334:
                        if (position.x < 272 && (outCard == 6335 || outCard == 6153 || outCard == 6154))
                            return kTransitionScrollToRight;
                        else if (outCard == 6335 || outCard == 6153 || outCard == 6154)
                            return kTransitionScrollToLeft;
                        else
                            return kTransitionDissolve;
                    case 6153:
                    case 6154:
                    case 6335:
                        if (position.x < 272 && (outCard == 6150 || outCard == 6334))
                            return kTransitionScrollToRight;
                        else if (outCard == 6150 || outCard == 6334 || outCard == 6337 || outCard == 6121)
                            return kTransitionScrollToLeft;
                        else
                            return kTransitionDissolve;
                    case 6158:
                    {
                        if (outCard == 6170)
                            return kTransitionScrollToLeft;
                        if (outCard == 6174)
                            return kTransitionScrollToRight;
                    }
                    case 6178:
                        if (outCard != 6179)
                            return kTransitionDissolve;
                    case 6179:
                    {
                        if (outCard == 6178 && position.x < 272)
                            return kTransitionScrollToRight;
                        else if (outCard == 6178)
                            return kTransitionScrollToLeft;
                    }
                    case 6204:
                        if (outCard == 6216)
                            return kTransitionDissolve;
                    case 6241:
                        if (outCard == 6249 || outCard == 6245)
                            return kTransitionDissolve;
                        if (outCard == 6240)
                            return kTransitionScrollToLeft;
                    case 6245:
                        if (outCard == 6249)
                            return kTransitionDissolve;
                    case 6248:
                        if (outCard == 6250)
                            return kTransitionScrollToLeft;
                    case 6260:
                    case 6262:
                        return kTransitionDissolve;
                    default:
                        break;
                }
            }
            else
            {
                switch (inCard)
                {
                    case 6117:
                    case 6118:
                    case 6119:
                    case 6120:
                    case 6327:
                    case 6329:
                    case 6330:
                    case 6331:
                    case 6333:
                        if (transition == kTransitionLeftToRight)
                            return kTransitionPushToRight;
                        else if (transition == kTransitionSlideToLeft)
                            return transition;
                        else
                            return kTransitionCopy;
                    default:
                        break;
                }
            }
            switch (outCard)
            {
                case 6144:
                case 6145:
                case 6146:
                case 6147:
                {
                    if (inCard == 6148)
                        return kTransitionScrollToRight;
                    else if (inCard == 6149)
                        return kTransitionScrollToLeft;
                    else
                        return kTransitionDissolve;
                }
                case 6150:
                case 6260:
                case 6262:
                case 6334:
                        return kTransitionDissolve;
                case 6157:
                case 6158:
                case 6263:
                case 7178:
                    return kTransitionDissolve;
                case 6244:
                    if (inCard == 6247)
                        return kTransitionDissolve;
                case 6238:
                    if (inCard == 6243)
                        return kTransitionScrollToLeft;
                    else if (inCard == 6241)
                        return kTransitionScrollToRight;
                    else
                        return kTransitionDissolve;
                case 6119:
                {
                    if (inCard == 6172)
                        return kTransitionScrollToLeft;
                    else
                        return kTransitionCopy;
                }
                case 6120:
                case 6330:
                    if (inCard == 6153 || inCard == 6154)
                        return kTransitionScrollToLeft;
                default:
                    break;
            }
        }
        case kStoneshipStack:
        {
            switch (inCard)
            {
                case 2004:
                {
                    if (transition == kTransitionCopy)
                        return kTransitionDissolve;
                    break;
                }
                case 2055:
                case 2056:
                    return kTransitionDissolve;
                case 2136:
                case 2137:
                    return kTransitionScrollToBottom;
                case 2138:
                case 2139:
                case 2142:
                    if (transition == kTransitionCopy)
                        return kTransitionScrollToBottom;
                    break;
                case 2146:
                    if (outCard == 2127 || outCard == 2128)
                        return kTransitionScrollToBottom;
                    break;
                case 2175:
                    return kTransitionScrollToTop;
                case 2108:
                case 2109:
                case 2111:
                    return kTransitionScrollToBottom;
                case 2080:
                    if (outCard == 2081)
                        return kTransitionScrollToBottom;
                    break;
                case 2088:
                case 2327:
                    if (outCard == 2325 || outCard == 2326 || outCard == 2250)
                        return kTransitionScrollToLeft;
                    else if (outCard == 2319 || outCard == 2320 || outCard == 2288)
                        return kTransitionScrollToRight;
                case 2226:
                    if (outCard == 2229 && position.x < 272)
                        return kTransitionScrollToRight;
                    else if (outCard == 2229)
                        return kTransitionScrollToLeft;
                case 2390:
                    if (outCard == 2047 || outCard == 2048 || outCard == 2049 || outCard == 2050)
                        return kTransitionDissolve;
                default:
                    break;
                    
            }
            switch (outCard)
            {
                case 2004:
                {
                    if (transition == kTransitionCopy)
                        return kTransitionDissolve;
                    break;
                }
                case 2055:
                case 2056:
                    return kTransitionDissolve;
                case 2136:
                case 2137:
                    return kTransitionScrollToTop;
                case 2138:
                    if (transition == kTransitionBottomToTop)
                        return kTransitionDissolve;
                    if (transition == kTransitionCopy)
                        return kTransitionScrollToTop;
                    break;
                case 2139:
                case 2142:
                    if (transition == kTransitionCopy)
                        return kTransitionScrollToTop;
                    break;
                case 2146:
                    if (inCard == 2127 || inCard == 2128)
                        return kTransitionScrollToTop;
                    break;
                case 2171:
                    if (inCard == 2126)
                        return kTransitionDissolve;
                    break;
                case 2175:
                    return kTransitionScrollToBottom;
                case 2108:
                case 2109:
                case 2111:
                    return kTransitionScrollToTop;
                default:
                    break;
            }
        }
        case kChannelwoodStack:
        {
            if (outCard != 0)
            {
                switch (inCard)
                {
                    case 3246:
                    case 3247:
                    case 3252:
                    case 3256:
                    case 3259:
                    case 3262:
                    {
                        if (outCard == 3246 ||
                            outCard == 3247 ||
                            outCard == 3252 ||
                            outCard == 3256 ||
                            outCard == 3259 ||
                            outCard == 3262)
                            return kTransitionCopy;
                        break;
                    }
                    case 3555:
                    case 3556:
                            return kTransitionDissolve;
                        break;
                    default:
                        break;
                }
            }
            switch (inCard)
            {
                case 3012:
                case 3326:
                case 3327:
                case 3328:
                case 3690:
                case 3691:
                case 3692:
                case 3729:
                case 3730:
                case 3731:
                case 3768:
                case 3769:
                case 3770:
                case 3807:
                case 3808:
                case 3809:
                case 3846:
                case 3847:
                case 3848:
                    return kTransitionDissolve;
                case 3277:
                    return kTransitionScrollToBottom;
                default:
                    break;
            }
            switch (outCard)
            {
                case 3277:
                        return kTransitionScrollToTop;
                case 3556:
                        return kTransitionDissolve;
                    break;
            }
            default:
            	break;
        }
        case kDniStack:
        {
            if (inCard == 5013 || outCard == 5013 || inCard == 5017)
                return kTransitionDissolve;
        }
    }
    return transition;
    }
    
    uint16 stepFix(uint16 steps, MystStack stack, uint16 inCard, uint16 outCard, TransitionType transition, bool sameCard)
    {
        switch (stack)
        {
            case kMystStack:
            {
                switch (inCard)
                {
                    case 4138:
                    {
                        if (sameCard == true)
                            return steps * 3 / 2;
                        break;
                    }
                    case 4028:
                        if (outCard == 0)
                            return steps * 5 / 2;
                    case 4159:
                    case 4160:
                    case 4161:
                    case 4162:
                    case 4166:
                        return steps * 2;
                    case 4537:
                        if (outCard == 0)
                            return steps * 4;
                    case 4317:
                    case 4319:
                        if (outCard == 0)
                            return steps * 4;
                    case 4379:
                        if (outCard == 4383)
                            return steps * 2;
                    case 4383:
                        if (outCard == 4379)
                            return steps * 2;
                    case 4393:
                        if (outCard == 4394)
                            return steps * 4;
                    case 4394:
                        if (outCard == 4393)
                            return steps * 4;
                    case 4451:
                        if (outCard == 4452)
                            return steps * 2;
                    case 4452:
                        if (outCard == 4451)
                            return steps * 2;
                    default:
                        break;
                }
            }
            case kMechanicalStack:
            {
                switch (inCard)
                {
                    case 6117:
                    case 6118:
                    case 6119:
                    case 6120:
                    case 6327:
                    case 6329:
                    case 6330:
                    case 6331:
                    case 6333:
                        if (transition == kTransitionSlideToLeft || transition != kTransitionPushToRight)
                            return 40;
                    case 6151:
                    case 6156:
                        if (transition != kTransitionCopy)
                            return 40;
                    default:
                        break;
                }
            }
            case kSeleniticStack:
            {
                switch (inCard)
                {
                    case 1182:
                        if (transition == kTransitionSlideToBottom || transition == kTransitionTopToBottom || transition == kTransitionPushToBottom)
                            return steps / 3;
                    case 1139:
                    case 1140:
                        if (transition == kTransitionBottomToTop || kTransitionPushToTop || transition == kTransitionTopToBottom || kTransitionSlideToBottom)
                            return steps * 3 / 4;
                    default:
                        break;
                }
            default:
                break;
            }
        }
        return steps;
    };
};

}
#endif
