from pathlib import Path
from sqlalchemy.ext.asyncio import create_async_engine, AsyncSession
from sqlalchemy.orm import sessionmaker
from PoopBiter.utils import SingletonMeta
from Cherry.models import Base


class Database(metaclass=SingletonMeta):
    DATABASE_NAME = "cherry.db"
    def __init__(self, root: Path):
        self._root = root
        self._engine = create_async_engine(f"sqlite+aiosqlite:///{(self._root / self.DATABASE_NAME).as_posix()}", echo=True)
        self._session = sessionmaker(
            self._engine, class_=AsyncSession, expire_on_commit=False
        )

    async def init_db(self):
        async with self._engine.begin() as conn:
            await conn.run_sync(Base.metadata.create_all)

    @classmethod
    async def get_db(cls):
        async with Database()._session() as session:
            try:
                yield session
                await session.commit()
            except Exception:
                await session.rollback()
                raise
            finally:
                await session.close()
