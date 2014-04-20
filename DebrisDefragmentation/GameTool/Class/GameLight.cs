﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GameTool.Class
{
    class GameLight : GameObject
    {
        DDWrapper.GameLight m_Light = null;

        public GameLight()
        {
            m_Light = new DDWrapper.GameLight();
        }

        public override void AttachParent(GameTool.Class.GameObject parent)
        {
            throw new NotImplementedException();
        }

        public override void AddChild(GameTool.Class.GameObject child)
        {
            throw new NotImplementedException();
        }

        public override DDWrapper.GameObject Unwrapping()
        {
            return m_Light;
        }
    }
}
