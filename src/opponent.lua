-- refer opponent.tcl and opponent.scm

world = {
   width = 0,
   height = 0,
}

-- player's thrust is not used in this script now
player = {
   x = 0,
   y = 0,
   angle = 0,
   accel = 0,
   thrust = {
      forward = 0,
      reverse = 0,
   }
}

opponent = {
   x = 0,
   y = 0,
   angle = 0,
   accel = 0,
   thrust = {
      forward = 0,
      reverse = 0,
   }
}

-- virtual target
target = {
   x = 0,
   y = 0,
   angle = 0,
}

-- attack 0, envade 1
state = 0

function dump(o)
   if type(o) == 'table' then
      local s = '{ '
      for k,v in pairs(o) do
         if type(k) ~= 'number' then k = '"'..k..'"' end
         s = s .. '['..k..'] = ' .. dump(v) .. ','
      end
      return s .. '} '
   else
      return tostring(o)
   end
end

-- NOTE: the subject of this script is opponent
function getAngleToTarget(target)
   -- TODO: modify scm, tcl code
   local deltaX = target.x - opponent.x;
   local deltaY = target.y - opponent.y;
   local theta = math.atan(-deltaY, deltaX)

   if theta < 0 then
      theta = 2 * math.pi + theta
   end

   return theta * 180 / math.pi
end

function getDistanceToTarget(target)
   local deltaX = opponent.x - target.x
   local deltaY = opponent.y - target.y

   return math.sqrt(deltaX ^ 2 + deltaY ^ 2)
end

function playOpponent()
   -- print("before playOpponent player ", dump(player))
   -- print("before playOpponent opponent ", dump(opponent))

   -- attack
   if state == 0 then
      target.x = player.x
      target.y = player.y

      local distance = getDistanceToTarget(target)

      if distance < 30 then
         state = 1
         -- TODO: -1?
         target.x = -1
      else
         if distance > 100 then
            opponent.accel = player.thrust.forward
         else
            if distance > 50 then
               opponent.accel = player.thrust.forward / 3
            else
               opponent.accel = 0
            end
         end

         if distance < 200 then
            -- TODO
            fireWeapon()
         end
      end
   else
      -- envade
      if math.abs(target.x - opponent.x) < 10 and math.abs(target.y - opponent.y) < 10 then
         state = 0
      else
         if target.x < 0 then
            target.x = math.random() * world.width
            target.y = math.random() * world.height
         else
            opponent.accel = player.thrust.forward
         end
      end
   end

   target.angle = getAngleToTarget(target)
   local arc = target.angle - opponent.angle

   if arc < 0 then
      arc = arc + 360
   end

   if arc < 180 then
      opponent.angle = opponent.angle + 3
   else
      opponent.angle = opponent.angle - 3
   end
   -- print("after playOpponent player ", dump(player))
   -- print("after playOpponent opponent ", dump(opponent))
end
