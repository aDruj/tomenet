-- File: lib/scpt/runecraft.lua

-- MASKS

R1 = bshl(255,0)
R2 = bshl(255,8)
PROJ = bor(R1,R2)
MODE = bshl(255,16)
TYPE = bshl(255,24)
WARN = 0

-- FLAGS

LITE = bshl(1,0)
DARK = bshl(1,1)
NEXU = bshl(1,2)
NETH = bshl(1,3)
CHAO = bshl(1,4)
MANA = bshl(1,5)
WARN = bor(WARN,bshl(1,6)) 
WARN = bor(WARN,bshl(1,7)) 

MINI = bshl(1,16)
LENG = bshl(1,17)
COMP = bshl(1,18)
MDRT = bshl(1,19)
ENHA = bshl(1,20)
EXPA = bshl(1,21)
BRIE = bshl(1,22)
MAXI = bshl(1,23)

BOLT = bshl(1,24)
CLOU = bshl(1,25)
BALL = bshl(1,26)
STRM = bshl(1,27)
CONE = bshl(1,28)
SURG = bshl(1,29)
WARN = bor(WARN,bshl(1,30)) 
WARN = bor(WARN,bshl(1,31)) 

-- TABLES

R = { -- Rune Skill
[LITE] = { "Light",    SKILL_R_LITE },
[DARK] = { "Darkness", SKILL_R_DARK },
[NEXU] = { "Nexus",    SKILL_R_NEXU },
[NETH] = { "Nether",   SKILL_R_NETH },
[CHAO] = { "Chaos",    SKILL_R_CHAO },
[MANA] = { "Mana",     SKILL_R_MANA }}

P = { -- Projection GF_TYPE Weight Colour
[LITE]           = { "light",           GF_LITE,       400, "L" },
[DARK]           = { "darkness",        GF_DARK,       550, "A" },
[NEXU]           = { "nexus",           GF_NEXUS,      250, "x" }, -- ADD tele,ruination
[NETH]           = { "nether",          GF_NETHER,     550, "n" }, -- ADD drain (invert GF_EXP)
[CHAO]           = { "chaos",           GF_CHAOS,      600, "m" }, -- ADD drain
[MANA]           = { "mana",            GF_MANA,       600, "N" },
[bor(LITE,DARK)] = { "confusion",       GF_CONFUSION,  400, "C" },
[bor(LITE,NEXU)] = { "inertia",         GF_INERTIA,    200, "q" },
[bor(LITE,NETH)] = { "electricity",     GF_ELEC,      1200, "e" },
[bor(LITE,CHAO)] = { "fire",            GF_FIRE,      1200, "f" },
[bor(LITE,MANA)] = { "water",           GF_WATER,      300, "Y" }, -- FIX confusion,stun
[bor(DARK,NEXU)] = { "gravity",         GF_GRAVITY,    150, "V" }, -- FIX slow,stun,phase
[bor(DARK,NETH)] = { "cold",            GF_COLD,      1200, "c" },
[bor(DARK,CHAO)] = { "acid",            GF_ACID,      1200, "a" },
[bor(DARK,MANA)] = { "poison",          GF_POIS,       800, "p" }, -- ADD m_ptr->poisoned
[bor(NEXU,NETH)] = { "time",            GF_TIME,       150, "t" }, -- ADD drain,ruination
[bor(NEXU,CHAO)] = { "sound",           GF_SOUND,      400, "S" },
[bor(NEXU,MANA)] = { "shards",          GF_SHARDS,     400, "H" }, -- ADD m_ptr->bleeding
[bor(NETH,CHAO)] = { "hellfire",        GF_HELLFIRE,   800, "X" },
[bor(NETH,MANA)] = { "force",           GF_FORCE,      250, "F" },
[bor(CHAO,MANA)] = { "disenchantment",  GF_DISENCHANT, 500, "T" }} -- ADD unpower? ^^ Players too!

M = { -- Mode Level Cost Fail Damage Radius Duration Energy
[MINI] = { "minimized",   0,  6, -10,  6, -1,  8, 10 },
[LENG] = { "lengthened",  2,  8,  -5,  8,  0, 14, 10 },
[COMP] = { "compressed",  3,  9,   0,  9, -2, 12, 10 }, 
[MDRT] = { "moderate",    5, 10,   0, 10,  0, 10, 10 },
[ENHA] = { "enhanced",    5, 10,   5, 10,  0, 10, 10 },
[EXPA] = { "expanded",    7, 12,  10,  9,  2,  8, 10 },
[BRIE] = { "brief",       8,  7,  15,  6,  0,  6,  5 },
[MAXI] = { "maximized",  10, 18,  20, 14,  1, 12, 10 }}

T = { -- Type Level Cost Max Dice Max Damage Max Radius Max Duration Max
[BOLT] = { "bolt",    5,  1, 15, 1, 73,  2,  20, 0,  0,  0,  0},
[CLOU] = { "cloud",  10,  5, 50, 0,  0,  3,  90, 2,  2,  4, 14},
[BALL] = { "ball",   15, 10, 35, 0,  0, 90, 519, 3,  3,  0,  0},
[STRM] = { "storm",  20, 30, 50, 0,  0, 34, 266, 1,  1, 28, 40},
[CONE] = { "cone",   25, 20, 40, 1, 73,  2,  20, 3,  3,  0,  0},
[SURG] = { "surge",  30, 20, 50, 0,  0, 30, 240, 7, 13,  0,  0}}

E = { -- Enhanced Level Cost Max Dice Max Damage Max Radius Max Duration Max
[BOLT] = { "beam",   10,  5, 25, 1, 73,  2,  20, 0,  0,  0,  0},
[CLOU] = { "vortex", 15, 10, 40, 0,  0,  3,  90, 1,  1,  8, 20},
[BALL] = { "burst",  20, 15, 40, 0,  0, 90, 519, 3,  3,  0,  0},
[STRM] = { "nimbus", 25, 30, 50, 0,  0, 26,  56, 1,  1, 30, 75},
[CONE] = { "shot",   30,  3, 30, 1, 73,  2,  10, 9,  9,  0,  0},
[SURG] = { "glyph",  35, 10, 25, 0,  0, 30, 240, 0,  0,  0,  0}}

-- HACKS

adj_mag_fail = {99,99,99,99,99,50,30,20,15,12,11,10,9,8,7,6,6,5,5,5,4,4,4,4,3,3,2,2,2,2,1,1,1,1,1,0,0,0}
adj_mag_stat = {0,0,0,1,1,1,2,2,3,3,4,4,5,6,7,8,9,10,11,12,13,14,16,18,21,24,27,30,33,36,39,42,45,48,51,54,57,60}

-- COMMON

function rspell_skill(I,u)
  local p = I~=0 and players(I) or player
  local a = p.s_info[1 + R[band(u,R1)][2]].value / 1000
  local b = p.s_info[1 + R[bshr(band(u,R2),8)][2]].value / 1000
  if a < b then
    return a
  else
    return b
  end
end

function rspell_level(u)
  return M[band(u,MODE)][2] + T[band(u,TYPE)][2]
end

function rspell_ability(s,l)
  return s - l + 1
end

function rspell_scale(s,l,h)
  return l + ((h - l) * s / 50)
end

function rspell_cost(u,s)
  local XX = band(u,ENHA)~=0 and E[band(u,TYPE)] or T[band(u,TYPE)]
  local l = XX[3]
  local x = rspell_scale(s, l, XX[4]) * M[band(u,MODE)][3] / 10
  return x < l and l or x
end

function rspell_failure(p,u,x,c)
  if x < 1 or p.csp < c then
    return 100
  else
    x = 15 - (x > 15 and 15 or x)
    x = x * 3 + 5 + M[band(u,MODE)][4]
  end
  x = x - (((adj_mag_stat[p.stat_ind[1+A_INT]] * 65 + adj_mag_stat[p.stat_ind[1+A_DEX]] * 35) / 100) - 3)
  local minfail = (adj_mag_fail[p.stat_ind[1+A_INT]] * 65 + adj_mag_fail[p.stat_ind[1+A_DEX]] * 35) / 100
  if x < minfail then x = minfail end
  if p.blind~=0 then x = x + 10 end
  if p.stun > 50 then
    x = x + 25
  elseif p.stun~=0 then
    x = x + 15
  end
  return x > 95 and 95 or x
end

function rspell_sval(u)
  return bor(band(u,R1),bshr(band(u,R2),8))
end

function rspell_damage(u,s)
  local XX = band(u,ENHA)~=0 and E[band(u,TYPE)] or T[band(u,TYPE)]
	local w = (P[rspell_sval(u)][3] * 25 + 1200 * (100 - 25)) / 100
  local m = M[band(u,MODE)][5]
  local x = rspell_scale(s, XX[5], XX[6] * w / 1200)
  local y = rspell_scale(s, XX[7], XX[8] * m / 10)
	local d = rspell_scale(s, XX[7], ((XX[8] * w) / 1200) * m / 10)
  return x,y,d
end

function rspell_radius(u,s)
  local XX = band(u,ENHA)~=0 and E[band(u,TYPE)] or T[band(u,TYPE)]
  local x = rspell_scale(s, XX[9], XX[10]) + M[band(u,MODE)][6]
  return x < 1 and 1 or x
end

function rspell_duration(u,s)
  local XX = band(u,ENHA)~=0 and E[band(u,TYPE)] or T[band(u,TYPE)]
  local x = rspell_scale(s, XX[11], XX[12]) * M[band(u,MODE)][7] / 10
  return x < 3 and 3 or x
end

function rspell_energy(I,u)
  local p = I~=0 and players(I) or player
	return level_speed(p.wpos) * M[band(u,MODE)][8] / 10
end

-- CLIENT

function rcraft_end(u)
  return band(u,TYPE)
end

function rcraft_com(u)
  if band(u,MODE)~=0 then
    return "(Types a-f, *=List, ESC=exit) Which type? "
  elseif band(u,R2)~=0 then
    return "(Modes a-g, *=List, ESC=exit) Which mode? "
  else
    return "(Runes a-f, *=List, ESC=exit) Which rune? "
  end
end

function rcraft_max(u)
  if band(u,MODE)~=0 then
    return 5
  elseif band(u,R2)~=0 then
    return 7
  else
    return 5
  end
end

function rcraft_prt(u,w)
  local U,C,row,col
  if w~=0 then
    C = TERM_GREEN
    row,col = 9,16
  else
    C = TERM_WHITE
    row,col = 1,13
  end
  if band(u,MODE)~=0 then
    if w~=0 then
      c_prt(C,"Please choose the component type of the spell.",row,col)
      row = row + 1
      c_prt(C,"ie. Select the spell form:",row,col)
      row = row + 2
    end
    c_prt(C,"   Type     Level Cost Fail Info",row,col)
    local XX,l,s,a,c,f,x,y,d,r,t
    local X = (band(u,ENHA)==0)
    local p = player
    for i = 0,rcraft_max(u) do
      U = bshl(1,i+24)
      XX = X and T[U] or E[U]
      U = bor(u,U)
      l = rspell_level(U)
      s = rspell_skill(0,U)
      a = rspell_ability(s,l)
      c = rspell_cost(U,s)
      f = rspell_failure(p,U,a,(w~=0) and 0 or c)
      x,y,d = rspell_damage(U,s)
      r = rspell_radius(U,s)
      t = rspell_duration(U,s)
      if a > 0 then
        C = TERM_L_GREEN
        if w==0 then
          if p.stun~=0 or p.blind~=0 then C = TERM_YELLOW end
          if p.csp < c then C = TERM_ORANGE end
          if p.antimagic~=0 and p.admin_dm==0 then C = TERM_RED end -- UNHACK
          if p.anti_magic~=0 then C = TERM_RED end
        end
      else
        C = TERM_L_DARK
      end
      if band(U,BOLT)~=0 then
        c_prt(C, format("%c) %-8s %5d %4d %3d%% dam %dd%d",
          strbyte('a')+i, XX[1], a, c, f,
          x, y),
        row+i+1, col)
      elseif band(U,CLOU)~=0 then
        c_prt(C, format("%c) %-8s %5d %4d %3d%% dam %d rad %d dur %d",
          strbyte('a')+i, XX[1], a, c, f,
          d, r, t),
        row+i+1, col)
      elseif band(U,BALL)~=0 then
        c_prt(C, format("%c) %-8s %5d %4d %3d%% dam %d rad %d",
          strbyte('a')+i, XX[1], a, c, f,
          d, r),
        row+i+1, col)
      elseif band(U,STRM)~=0 then
        c_prt(C, format("%c) %-8s %5d %4d %3d%% dam %d rad %d dur %d",
          strbyte('a')+i, XX[1], a, c, f,
          d, r, t),
        row+i+1, col)
      elseif band(U,CONE)~=0 then
        c_prt(C, format("%c) %-8s %5d %4d %3d%% dam %dd%d%s rad %d",
          strbyte('a')+i, XX[1], a, c, f,
          x, y, X and "" or " (x5)", r),
        row+i+1, col)
      elseif band(U,SURG)~=0 then
        if X then
          c_prt(C, format("%c) %-8s %5d %4d %3d%% dam %d (x3) rad %d",
            strbyte('a')+i, XX[1], a, c, f,
            d, r),
          row+i+1, col)
        else
          c_prt(C, format("%c) %-8s %5d %4d %3d%% dam %d",
            strbyte('a')+i, XX[1], a, c, f,
            d),
          row+i+1, col)
        end
      end
    end
  elseif band(u,R2)~=0 then
    if w~=0 then
      c_prt(C,"Please choose the component mode of the spell.",row,col)
      row = row + 1
      c_prt(C,"ie. Select the spell modifier:",row,col)
      row = row + 2
      col = col - 6
    end
    c_prt(C,"   Mode       Level Cost Fail Damage Radius Duration Energy",row,col)
    local MM,a
    for i = 0,rcraft_max(u) do
      U = bshl(1,i+16)
      MM = M[U]
      U = bor(bor(u,U),BOLT)
      l = rspell_level(U)
      s = rspell_skill(0,U)
      a = rspell_ability(s,l)
      c_prt(a < 1 and TERM_L_DARK or TERM_L_GREEN,
      format("%c) %-10s %+5d %3d%% %+3d%% %5d%% %+6d %7d%% %5d%%",
        strbyte('a')+i,
        MM[1],
        MM[2],
        MM[3] * 10,
        MM[4],
        MM[5] * 10,
        MM[6],
        MM[7] * 10,
        MM[8] * 10),
      row+i+1, col)
    end
  else
    if w~=0 then
      c_prt(C,"Please choose the component runes of the spell.",row,col)
      row = row + 1
      c_prt(C,"ie. Select the spell runes to combine:",row,col)
      row = row + 2
      c_prt(C,"(Select the same rune twice to cast a single rune spell!)",row,col-5)
      row = row + 2
    end
    c_prt(C, "   Rune", row, col)
    for i = 0,rcraft_max(u) do
      U = bshl(1,i)
      if band(u,R1)~=0 then
        U = bor(bor(bor(bshl(u,8),U),MINI),BOLT)
      else
        U = bor(bor(bor(bshl(U,8),U),MINI),BOLT)
      end
      l = rspell_level(U)
      s = rspell_skill(0,U)
      a = rspell_ability(s,l)
      C = a < 1 and TERM_L_DARK or TERM_L_GREEN
      U = bshl(1,i)
      c_prt(band(u,R1)~=0 and band(u,U)~=0 and TERM_L_UMBER or C,
        format("%c) %-11s",
        strbyte('a')+i,
        R[U][1]),
      row+i+1, col)
    end
  end
end

function rcraft_bit(u,i)
  local U = bshl(1,i)
  if band(u,MODE)~=0 then
    return bshl(U,24)
  elseif band(u,R2)~=0 then
    return bshl(U,16)
  elseif band(u,R1)~=0 then
    return bshl(U,8)
  else
    return bshl(U,0)
  end
end

function rcraft_dir(u)
  return band(u,bor(STRM,SURG))==0
end

function rcraft_ftk(u)
  return band(u,bor(bor(CLOU,STRM),SURG))==0
end

function rcraft_arr(u)
  if band(u,ENHA)~=0 and band(u,bor(STRM,SURG))~=0 then
    return 0
  else
    return 1
  end
end

-- SERVER

function cast_rune_spell(I,D,u)
  if band(u,WARN)~=0 then return end
  if band(u,PROJ)==0 then return end
  if band(u,MODE)==0 then return end
  if band(u,TYPE)==0 then return end
  local p = players(I)
  local e = rspell_energy(I,u)
  if p.confused~=0 then
		msg_print(I,"You are too confused!")
		p.energy = p.energy - e
		return 0
	end
  if p.antimagic~=0 and p.admin_dm==0 then
		p.energy = p.energy - e
		msg_print(I,"\255wYour anti-magic field disrupts any magic attempts.")
		return 0
	end
	if p.anti_magic~=0 then
		p.energy = p.energy - e
		msg_print(I,"\255wYour anti-magic shell disrupts any magic attempts.")
		return 0
	end
  if check_antimagic(I,100)~=0 then
		p.energy = p.energy - e
		return 1
	end
  local v = rspell_sval(u)
  local PP = P[v]
  local MM = M[band(u,MODE)]
  local X = (band(u,ENHA)==0)
  local XX = X and T[band(u,TYPE)] or E[band(u,TYPE)]
  local l = rspell_level(u)
  local s = rspell_skill(I,u)
  local a = rspell_ability(s,l)
  local S = X and format("%s %s %s",MM[1],PP[1],XX[1]) or format("%s %s",PP[1],XX[1])
  -- local S = X and format("%s %s of %s",MM[1],XX[1],PP[1]) or format("%s of %s",XX[1],PP[1])
  if a < 1 then
    msg_print(I,format("\255sYour skill is not high enough! (%s; level: %d)",S,a))
		return 0
	end
  local c = rspell_cost(u,s)
	if p.csp < c then
		msg_print(I,format("\255oYou do not have enough mana. (%s; cost: %d)",S,c))
		p.energy = p.energy - e
    return 0
	end
  local f = rspell_failure(p,u,a,c)
  local x,y,d = rspell_damage(u,s)
  local b = damroll(x,y)
  d = d > b and d or b
  if band(u,SURG)~=0 and band(u,ENHA)~=0 and warding_rune(I, PP[2], d)==0 then
    return 0
  end
	if magik(f)~=0 then
    b = d / 5 + 1
  else
    b = 0
  end
  local SS = band(u,EXPA)~=0 and "an" or "a"
	msg_print(I,format("You %strace %s %s.", b > 0 and "\255Rincompetently\255w " or "",SS,S))
  p.attacker = format(" traces %s %s for", SS, S)
  p.energy = p.energy - e
	p.csp = p.csp - c
  if b > 0 then
    project(PROJECTOR_RUNE, 0, p.wpos, p.py, p.px, b, PP[2], bor(bor(bor(bor(bor(PROJECT_KILL,PROJECT_NORF),PROJECT_JUMP),PROJECT_RNAF),PROJECT_NODO),PROJECT_NODF), "")
  end
  local r = rspell_radius(u,s)
  local t = rspell_duration(u,s)
	if band(u,BOLT)~=0 then
    if X then
      fire_bolt(I, PP[2], D, d, p.attacker)
    else
      fire_beam(I, PP[2], D, d, p.attacker)
    end
  elseif band(u,CLOU)~=0 then
    if X then
      fire_cloud(I, PP[2], D, d, r, t, 9, p.attacker)
    else
      fire_wave(I, PP[2], D, d, r, t, 10, EFF_VORTEX, p.attacker)
    end
  elseif band(u,BALL)~=0 then
    -- Easter Egg? Illuminate (or unlite) your position by firing a ball up!
    local tx = p.px
    local ty = p.py
    if D==5 and target_okay(I)~=0 then
      tx = p.target_col
      ty = p.target_row
    end
    if X then
      fire_ball(I, PP[2], D, d, r, p.attacker)
    else
      fire_burst(I, PP[2], D, d, r, p.attacker)
    end
    if tx==p.px and ty==p.py then
      if band(rspell_sval(u),LITE)~=0 then lite_room(I, p.wpos, p.py, p.px) end
      if band(rspell_sval(u),DARK)~=0 then unlite_room(I, p.wpos, p.py, p.px) end
    end
  elseif band(u,STRM)~=0 then
    if X then
      fire_wave(I, PP[2], 0, d, r, t, 10, EFF_STORM, p.attacker)
    else
      if (p.nimbus == 0) or (p.nimbus_t ~= PP[2]) then
        msg_print(I,format("\255WYou are wreathed with an aura of powerful \255%s%s\255W!", PP[4], PP[1]));
      end
      set_nimbus(I, t, PP[2], d)
    end
  elseif band(u,CONE)~=0 then
    if X then
      fire_cone(I, PP[2], D, d, r, p.attacker)
    else
      fire_shot(I, PP[2], D, x, y, r, 5, p.attacker)
    end
  elseif band(u,SURG)~=0 then
    fire_wave(I, PP[2], 0, d, 1, r, 1, EFF_WAVE, p.attacker)
  end
  p.redraw = bor(p.redraw,PR_MANA)
  return 1
end

function rcraft_name(v)
  return P[v][1]
end

function rcraft_type(v)
  return P[v][2]
end

function rcraft_code(v)
  return P[v][4]
end

function rcraft_rune(I,v)
  local p = I~=0 and players(I) or player
  local x = 51
  local s
  if band(v,LITE)~=0 then
    s = p.s_info[1 + R[LITE][2]].value / 1000
    x = (s < x) and s or x
  end
  if band(v,DARK)~=0 then
    s = p.s_info[1 + R[DARK][2]].value / 1000
    x = (s < x) and s or x
  end
  if band(v,NEXU)~=0 then
    s = p.s_info[1 + R[NEXU][2]].value / 1000
    x = (s < x) and s or x
  end
  if band(v,NETH)~=0 then
    s = p.s_info[1 + R[NETH][2]].value / 1000
    x = (s < x) and s or x
  end
  if band(v,CHAO)~=0 then
    s = p.s_info[1 + R[CHAO][2]].value / 1000
    x = (s < x) and s or x
  end
  if band(v,MANA)~=0 then
    s = p.s_info[1 + R[MANA][2]].value / 1000
    x = (s < x) and s or x
  end
  if x < 40 or x > 50 then
    return 0
  else
    return x * 2
  end
end