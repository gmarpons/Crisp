-- PrologSyntax.hs  ------------------------------------------------------------

-- Copyright (C) 2011, 2012 Guillem Marpons <gmarpons@babel.ls.fi.upm.es>
--
-- This file is part of Crisp.
--
-- Crisp is free software: you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation, either version 3 of the License, or
-- (at your option) any later version.
--
-- Crisp is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with Crisp.  If not, see <http://www.gnu.org/licenses/>.

{-# LANGUAGE 
  NoImplicitPrelude,
  OverloadedStrings
  #-}

module Crisp.PrologSyntax (
                   Term (..)
                 , Atom (..)
                 , Literal (..)
                 , EPC (), mkHC, mkEPC, mkGPC
                 , Formula (..)
                 , mkConj
                 , EP
                 , emptyEP, addEPC, addEPCs, unionEP
                 , transLloydTopor
                 , headsOfTerms, headOfTerm
                 , ShowText (show)
                 )
where

import ClassyPrelude hiding (delete, show)
import qualified ClassyPrelude as CP
    (show)
import Control.Monad
import Data.List
    (delete, foldl, foldr1)
import Data.Text
    (Text)
-- import Text.Show

-- infixr 5 ++
-- (++) :: Text -> Text -> Text
-- t1 ++ t2 = t1 `append` t2

data Term = Var Text
          | Structure Text [Term]
          | List [Term]
          | Tuple [Term]
          | Integer Integer
            deriving (Eq, Show)

data Atom = Atom Text [Term]
            deriving (Eq, Show)

data Literal = Pos Atom
             | Neg Atom
               deriving (Eq)

--  Clauses in general programs
data GPC = GPC Atom [Literal]
         | GPCDec Text
           deriving (Eq)

newtype GP = GP [GPC]
             deriving (Eq)

--  Clauses in extended programs.
--
--  The meaning of EPC head a b is head :- a, b where a is a
--  conjunction of literals and b is an arbitrary first order formula
data EPC = EPC Atom [Literal] [Formula]
           deriving (Eq)

data Formula = Top
             | Bot
             | At Atom
             | Not Formula
             | Conj Formula Formula
             | Disj Formula Formula
             | Impl Formula Formula
             | Equiv Formula Formula
             | Forall Text Formula
             | Exists Text Formula
               deriving (Eq, Show)

newtype EP = EP [EPC]
             deriving (Eq)

emptyEP :: Monad m => m EP
emptyEP = return $ EP []

addEPC :: Monad m => EPC -> EP -> m EP
addEPC epc (EP epcs) = return $ EP (epc : epcs)

addEPCs :: Monad m => [EPC] -> EP -> m EP
addEPCs epcs ep = foldM (flip addEPC) ep epcs

unionEP :: Monad m => EP -> EP -> m EP
unionEP ep (EP epcs) = addEPCs epcs ep

-- | Creates a Horn clause.
mkHC :: Atom -> [Atom] -> EPC
mkHC h b = EPC h (map Pos b) []

-- | Creates a general program clause.
mkGPC :: Atom -> [Literal] -> EPC
mkGPC h b = EPC h b []

-- | Creates an extended program clause.
mkEPC :: Atom -> Formula -> EPC
mkEPC h b = EPC h [] [b]

-- | Creates a conjunction of formulas.
mkConj :: [Formula] -> Formula
mkConj fs = foldl Conj Top fs

-- Lloyd-Topor Transformation

transLloydTopor :: EP -> GP
transLloydTopor (EP epcs) =
    GP (concat (map tlp epcs))

tlp :: EPC -> [GPC]

tlp (EPC h a []) =
    [GPC h a]

tlp (EPC h a (Top : b)) =
    tlp (EPC h a b)

tlp (EPC _ _ (Bot : _)) =
    []

tlp (EPC h a (At at : b)) =
    tlp (EPC h (a ++ [Pos at]) b)

tlp (EPC _ _ (Not Top : _)) =
    []

tlp (EPC h a (Not Bot : b)) =
    tlp (EPC h a b)

tlp (EPC h a (Not (At at) : b)) =
    tlp (EPC h (a ++ [Neg at]) b)

tlp (EPC h a (Not (Not w) : b)) =
    tlp (EPC h a (w : b))

tlp (EPC h a (Not (Conj v w) : b)) =
    tlp (EPC h a (Not v : b))
    ++ tlp (EPC h a (Not w : b))

tlp (EPC h a (Not (Disj v w) : b)) =
    tlp (EPC h a (Conj (Not v) (Not w) : b))

tlp (EPC h a (Not (Impl w v) : b)) =
    tlp (EPC h a (w : Not v : b))

tlp (EPC h a (Not (Forall x w) : b)) =
    tlp (EPC h a (Exists x (Not w) : b))

tlp (EPC h a (Not (Exists x w) : b)) =
    --  TODO: Lloyd-Topor transformation for existance quantification
    --  needs to generate fresh predicate names. To avoid duplicated
    --  clauses we need to create those predicates and clauses just
    --  once.
    let an = auxPredName h
        fvs = freeVars w
        h' = Atom an (map Var fvs)
    in mkDiscontiguous an (length fvs)
       ++ tlp (EPC h a (At h' : b))
       ++ tlp (EPC h' [] (Exists x w : []))

tlp (EPC h a (Conj v w : b)) =
    tlp (EPC h a (v : w : b))

tlp (EPC h a (Disj v w : b)) =
    tlp (EPC h a (v : b))
    ++ tlp (EPC h a (w : b))

tlp (EPC h a (Impl v w : b)) =
    tlp (EPC h a (Not v : b))
    ++ tlp (EPC h a (w : b))

tlp (EPC h a (Equiv v w : b)) =
    --  TODO: generation never clashing predicate names needed
    let an = auxPredName h
        anV = an ++ "_V"
        anW = an ++ "_W"
        fvsV = freeVars v
        fvsW = freeVars w
        hV = Atom anV (map Var fvsV)
        hW = Atom anW (map Var fvsW)
    in tlp (EPC h a ( Disj (Conj (At hV) (At hW))
                           (Conj (Not $ At hV) (Not $ At hW))
                    : b))
       ++ mkDiscontiguous anV (length fvsV)
       ++ tlp (EPC hV [] [v])
       ++ mkDiscontiguous anW (length fvsW)
       ++ tlp (EPC hW [] [w])

tlp (EPC _ _ (Not (Equiv _ _) : _)) = error "Cannot translate not equiv"

tlp (EPC h a (Forall x w : b)) =
    tlp (EPC h a (Not (Exists x (Not w)) : b))

tlp (EPC h a (Exists _ w : b)) =
    tlp (EPC h a (w : b))

freeVars :: Formula -> [Text]
freeVars Top = []
freeVars Bot = []
freeVars (At at) =
    varsInAtom at
freeVars (Not w) =
    freeVars w
freeVars (Conj v w) =
    union (freeVars v) (freeVars w)
freeVars (Disj v w) =
    union (freeVars v) (freeVars w)
freeVars (Impl w v) =
    union (freeVars w) (freeVars v)
freeVars (Equiv w v) =
    union (freeVars w) (freeVars v)
freeVars (Forall x w) =
    delete x (freeVars w)
freeVars (Exists x w) =
    delete x (freeVars w)

varsInTerm :: Term -> [Text]
varsInTerm (Var x) = [x]
varsInTerm (Structure _ ts) = foldl union [] (map varsInTerm ts)
varsInTerm (List ts) = foldl union [] (map varsInTerm ts)
varsInTerm (Tuple ts) = foldl union [] (map varsInTerm ts)
varsInTerm (Integer _) = []

varsInAtom :: Atom -> [Text]
varsInAtom (Atom _ ts) = foldl union [] (map varsInTerm ts)

class ShowText a where
    show :: a -> Text

instance ShowText GP where
    show (GP gpcs) = showAnyListWith show "" "\n\n" "" gpcs

instance ShowText GPC where
    show (GPC h b) = show h ++
                     if null b
                     then "."
                     else " :-\n"
                          ++ showAnyListWith show "   " ",\n   " "." b
    show (GPCDec d) = ":- " ++ d ++ "."

instance ShowText Literal where
    show (Pos a) = show a
    show (Neg a) = "\\+ " ++ show a

instance ShowText Atom where
    show (Atom p ts) = showQuoted p ++
                       if null ts
                       then ""
                       else showAnyListWith show "(" ", " ")" ts

instance ShowText Term where
    show (Var v) = v
    show (Structure f ts) = showQuoted f ++
                            if null ts
                            then ""
                            else showAnyListWith show "(" ", " ")" ts
    show (List ts) = showAnyListWith show "[" ", " "]" ts
    show (Tuple ts) = showAnyListWith show "(" ", " ")" ts
    show (Integer i) = CP.show i

-- | Given an atom, it generates an auxiliar predicate name to be used
-- in Lloyd-Topor.

-- Highly coupled with show in order to print an empty head of a
-- clause and discontiguous in prefix form
mkDiscontiguous :: Text -> Int -> [GPC]
mkDiscontiguous p n =
    [ GPCDec ("discontiguous " ++ p ++ "/" ++ CP.show n) ]

auxPredName :: Atom -> Text
auxPredName (Atom p ts) =
    p ++ "_" ++ headsOfTerms ts

headsOfTerms :: [Term] -> Text
headsOfTerms ts = showAnyListWith headOfTerm "" "_" "" ts

headOfTerm :: Term -> Text
headOfTerm (Var v) = v
headOfTerm (Structure f _) =
    case f of
      "<" -> "le"
      _ -> f
headOfTerm (List ts) = headsOfTerms ts
headOfTerm (Tuple ts) = headsOfTerms ts
headOfTerm (Integer i) = CP.show i

showAnyListWith :: (a -> Text) -> Text -> Text -> Text -> [a] -> Text
showAnyListWith howToShow beg sep end l =
  beg
  ++ (if null l
      then ""
      -- TODO: avoid using foldr1 partial function
      else foldr1 (\x s -> x ++ sep ++ s) (map howToShow l))
  ++ end

showListWith :: ShowText a => Text -> Text -> Text -> [a] -> Text
showListWith = showAnyListWith show

showQuoted :: Text -> Text
showQuoted s = "'" ++ s ++ "'"

-- | Replaces all instances of a value in a list by another value.
replace :: Eq a =>
           a   -- ^ Value to look for
        -> a   -- ^ Value to replace it with
        -> [a] -- ^ Input list
        -> [a] -- ^ Output list
replace x y = map (\z -> if z == x then y else z)
